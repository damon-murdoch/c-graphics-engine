#include "common.h"

void load_vjs(FILE * f, obj o) {

	if (f) {
		int stage = 0;

		int pt = 0, polys = 0;

		char line[256];
		char line_dup[256];
		char *tok;
		while (fgets(line, sizeof(line), f)) {
			strcpy(line_dup, line);
			if (line[0] == '/' && line[1] == '/') {
				stage++;
			}
			else if (line[0] == ' ' && line[1] == ' ') {

			}
			else {
				if (stage == 1) {
					tok = strtok(line_dup, ",\n");
					if (tok) o->NumPtsObj = atoi(tok);
					tok = strtok(NULL, ",\n");
					if (tok) o->NumPolysObj = atoi(tok);
				}
				if (stage == 2) {
					if (pt < o->NumPtsObj) {

						tok = strtok(line_dup, " \n");
						if (tok) {
							o->ObjectPoints[pt].x = atoi(tok);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].y = atoi(tok);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].z = atoi(tok);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].r = atoi(tok);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].g = atoi(tok);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].b = atoi(tok);
						}

						pt++;
					}
				}
				if (stage == 3) {
					int sides = 0;
					tok = strtok(line_dup, " \n");
					if (tok) {
						sides = atoi(tok);
					}
					tok = strtok(NULL, " \n");
					for (int i = 0; i < sides; i++) {
						if (tok) {
							o->ObjectPolys[polys].Vertices[i] = atoi(tok);
						}
						tok = strtok(NULL, " \n");
					}
					polys++;
				}
			}
		}
		find_centre_3d(o);
		o->ObjProps.Rotation.x = 0; //= { 0,0,0 };
		o->ObjProps.Rotation.y = 0;
		o->ObjProps.Rotation.z = 0;
		o->ObjProps.Scale.x = 100; // { 1,1,1 };
		o->ObjProps.Scale.y = 100;
		o->ObjProps.Scale.z = 100;
	}
	else return;
}

vec xyztoij(int x, int y, int z, vec res) {
	res->x = (DRAW_DISTANCE_CONSTANT*(x - (FRAME_WIDE) / 2) / (z + DRAW_DISTANCE_CONSTANT));
	res->y = (DRAW_DISTANCE_CONSTANT*(y - (FRAME_HIGH) / 2) / (z + DRAW_DISTANCE_CONSTANT));
	res->x += (FRAME_WIDE / 2);
	res->y += (FRAME_HIGH / 2);
	return res;
}

void print_object(obj o) {
	printf("%d,%d\n", o->NumPtsObj, o->NumPolysObj);
	printf("%f,%f,%f,%f,%f,%f\n",o->ObjProps.Scale.x,o->ObjProps.Scale.y,o->ObjProps.Scale.z,o->ObjProps.Rotation.x,o->ObjProps.Rotation.y,o->ObjProps.Rotation.z);
	for (int i = 0; i < o->NumPtsObj; i++) {
		printf("(%d,%d,%d,%d,%d,%d)\n", o->ObjectPoints[i].x, o->ObjectPoints[i].y, o->ObjectPoints[i].z, o->ObjectPoints[i].r, o->ObjectPoints[i].g, o->ObjectPoints[i].b);
	}
	for (int i = 0; i < o->NumPolysObj; i++) {
		printf("(%d,%d,%d,%d)\n", o->ObjectPolys[i].Vertices[0], o->ObjectPolys[i].Vertices[1], o->ObjectPolys[i].Vertices[2], o->ObjectPolys[i].Vertices[3]);
	}
}

void find_centre_3d(obj o) {
	int cx = 0, cy = 0, cz = 0;
	for (int i = 0; i < o->NumPtsObj; i++) {
		cx += o->ObjectPoints[i].x;
		cy += o->ObjectPoints[i].y;
		cz += o->ObjectPoints[i].z;
	}
	cx /= o->NumPtsObj;
	cy /= o->NumPtsObj;
	cz /= o->NumPtsObj;
	o->ObjProps.Center.x = cx;
	o->ObjProps.Center.y = cy;
	o->ObjProps.Center.z = cz;
	o->ObjProps.Center.r = 0;
	o->ObjProps.Center.g = 0;
	o->ObjProps.Center.b = 0;
}

void translate_obj(obj o, double dx, double dy, double dz) {
	for (int i = 0; i < o->NumPtsObj; i++) {
		o->ObjectPoints[i].x += dx;
		o->ObjectPoints[i].y += dy;
		o->ObjectPoints[i].z += dz;
	}
	o->ObjProps.Center.x += dx;
	o->ObjProps.Center.y += dy;
	o->ObjProps.Center.z += dz;
}

void move_to_point(obj o, double x, double y, double z){
	move_to_origin(o);
	translate_obj(o, x, y, z);
}

void move_to_origin(obj o) {
	translate_obj(o, +o->ObjProps.Center.x, +o->ObjProps.Center.y, +o->ObjProps.Center.z);
}

void scale_obj(obj o, double xscale, double yscale, double zscale) {
	double tx = o->ObjProps.Center.x, ty = o->ObjProps.Center.y, tz = o->ObjProps.Center.z;

	translate_obj(o, -tx, -ty, -tz);

	for (int i = 0; i < o->NumPtsObj; i++) {
		o->ObjectPoints[i].x *= xscale;
		o->ObjectPoints[i].y *= yscale;
		o->ObjectPoints[i].z *= zscale;
	}

	translate_obj(o, tx, ty, tz);
	o->ObjProps.Scale.x *= xscale;
	o->ObjProps.Scale.y *= yscale;
	o->ObjProps.Scale.z *= zscale;
}

void rotate_x(obj o, double theta) {
	double tx = o->ObjProps.Center.x, ty = o->ObjProps.Center.y, tz = o->ObjProps.Center.z;
	translate_obj(o, -tx, -ty, -tz);
	double new_x, new_y, new_z;
	for (int i = 0; i<o->NumPtsObj; i++) {
		new_x = o->ObjectPoints[i].x;
		new_y = (o->ObjectPoints[i].y * cos(theta)) - (o->ObjectPoints[i].z * sin(theta));
		new_z = (o->ObjectPoints[i].y * sin(theta)) + (o->ObjectPoints[i].z * cos(theta));
		o->ObjectPoints[i].x = (int)new_x;
		o->ObjectPoints[i].y = (int)new_y;
		o->ObjectPoints[i].z = (int)new_z;
	}
	translate_obj(o, tx, ty, tz);
	o->ObjProps.Rotation.x += theta;
}

void rotate_y(obj o, double theta) {
	double tx = o->ObjProps.Center.x, ty = o->ObjProps.Center.y, tz = o->ObjProps.Center.z;
	translate_obj(o, -tx, -ty, -tz);
	double new_x, new_y, new_z;
	for (int i = 0; i<o->NumPtsObj; i++) {
		new_x = (o->ObjectPoints[i].z * cos(theta)) - (o->ObjectPoints[i].x * sin(theta));
		new_y = o->ObjectPoints[i].y;
		new_z = (o->ObjectPoints[i].z * sin(theta)) + (o->ObjectPoints[i].x * cos(theta));
		o->ObjectPoints[i].x = (int)new_x;
		o->ObjectPoints[i].y = (int)new_y;
		o->ObjectPoints[i].z = (int)new_z;
	}
	translate_obj(o, tx, ty, tz);
	o->ObjProps.Rotation.y += theta;
}

void rotate_z(obj o, double theta) {
	double tx = o->ObjProps.Center.x, ty = o->ObjProps.Center.y, tz = o->ObjProps.Center.z;
	translate_obj(o, -tx, -ty, -tz);
	double new_x, new_y, new_z;
	for (int i = 0; i<o->NumPtsObj; i++) {
		new_x = (o->ObjectPoints[i].x * cos(theta)) - (o->ObjectPoints[i].y * sin(theta));
		new_y = (o->ObjectPoints[i].x * sin(theta)) + (o->ObjectPoints[i].y * cos(theta));
		new_z = o->ObjectPoints[i].z;
		o->ObjectPoints[i].x = new_x;
		o->ObjectPoints[i].y = new_y;
		o->ObjectPoints[i].z = new_z;
	}
	translate_obj(o, tx, ty, tz);
	o->ObjProps.Rotation.z += theta;
}

// unused functions written for backface culling algorithm here to demonstrate some level of understanding of the algorithm

void calculate_normals(obj o) {
	for (int i = 0; i < o->NumPolysObj; i++) {
		Point_3D_D t = { 0,0,0 };
		for (int j = 0; j < NumSidesPoly; j++) {
			t.x = t.x + ((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].y) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].y))*((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].z) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].z));
			t.y = t.y + ((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].z) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].z))*((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].x) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].x));
			t.z = t.z + ((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].x) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].x))*((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].y) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].y));
		}
		o->ObjectPolys[i].normals.x = t.x;
		o->ObjectPolys[i].normals.y = t.y;
		o->ObjectPolys[i].normals.z = t.z;
	}
}

void backface_culling(obj o) {
	Point_3D_D view = { FRAME_WIDE / 2,FRAME_HIGH / 2,0 };
	double dp;
	for (int i = 0; i < o->NumPolysObj; i++) {
		dp = (view.x * o->ObjectPolys[i].normals.x) + (view.y * o->ObjectPolys[i].normals.y) + (view.z * o->ObjectPolys[i].normals.z);
		if (dp >= 0) o->ObjectPolys[i].can_draw = 1;
		else o->ObjectPolys[i].can_draw = 0;
	}
}