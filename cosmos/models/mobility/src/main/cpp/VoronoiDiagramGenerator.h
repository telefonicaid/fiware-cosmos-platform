/*
* The author of this software is Steven Fortune.  Copyright (c) 1994 by AT&T
* Bell Laboratories.
* Permission to use, copy, modify, and distribute this software for any
* purpose without fee is hereby granted, provided that this entire notice
* is included in all copies of any software which is or includes a copy
* or modification of this software and in all copies of the supporting
* documentation for such software.
* THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
* REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
* OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
*/

/* 
* This code was originally written by Stephan Fortune in C code.  I, Shane O'Sullivan, 
* have since modified it, encapsulating it in a C++ class and, fixing memory leaks and 
* adding accessors to the Voronoi Edges.
* Permission to use, copy, modify, and distribute this software for any
* purpose without fee is hereby granted, provided that this entire notice
* is included in all copies of any software which is or includes a copy
* or modification of this software and in all copies of the supporting
* documentation for such software.
* THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
* REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
* OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
*/

#ifndef VORONOI_DIAGRAM_GENERATOR
#define VORONOI_DIAGRAM_GENERATOR

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <set>
#include <search.h>


#ifndef NULL
#define NULL 0
#endif
#define DELETED -2

#define le 0
#define re 1

#define DISTMIN 0.001

struct	Freenode	
{
	struct	Freenode *nextfree;
};

struct FreeNodeArrayList
{
	struct	Freenode* memory;
	struct	FreeNodeArrayList* next;

};

struct	Freelist	
{
	struct	Freenode	*head;
	int		nodesize;
};

struct Point	
{
	double x,y;
};

// structure used both for sites and for vertices 
struct Site	
{
	struct	Point	coord;
	int		sitenbr;
	int		refcnt;
};



struct Edge	
{
	double   a,b,c;
	struct	Site 	*ep[2];
	struct	Site	*reg[2];
	int		edgenbr;

};

struct GraphEdge
{
	int indv1, indv2, inds1, inds2;
    double xv1, yv1, xv2, yv2;
	struct GraphEdge* next;
};




struct Halfedge 
{
	struct	Halfedge	*ELleft, *ELright;
	struct	Edge	*ELedge;
	int		ELrefcnt;
	char	ELpm;
	struct	Site	*vertex;
	double	ystar;
	struct	Halfedge *PQnext;
};

struct pto_origen {        
    int num_lados;
    int num_adyacentes;
    int indice_lados;
    int indice_adyacentes;
    struct lado *poligono;
    int *ptos_adyacentes;
    double area;
};

struct vertice {
    double coordx;
    double coordy;
    int ind_orig;
    int ind_final;
};

struct lado {
    int indv1_orig;
    int indv2_orig;
    int indv1_final;
    int indv2_final;
};



class VoronoiDiagramGenerator
{
public:
	VoronoiDiagramGenerator();
	~VoronoiDiagramGenerator();

	bool generateVoronoi(double *xValues, double* yValues, int numPoints, double minX, double maxX, double minY, double maxY, double minDist=0);

  void postproceso(struct pto_origen **listaPuntos_sal, struct vertice **listaVertices_sal,
                 VoronoiDiagramGenerator *vdg, int count, int count_ext,
                 double xmin, double xmax, double ymin, double ymax, double distmin);
                 
  void junta_ent_cont(double **xValues, double **yValues, double *xValues_ext, double *yValues_ext,
                      int count, int count_ext);
                      
  void calc_lim(double *xmin, double*xmax, double *ymin, double *ymax, double *xValues, double *yValues, int count);
                 
	void resetIterator()
	{
		iteratorEdges = allEdges;
	}

	bool getNext(int& indv1, double& xv1, double& yv1, int& indv2, double& xv2, double& yv2, int& inds1, int& inds2)
	{
		if(iteratorEdges == 0)
			return false;
		
		indv1 = iteratorEdges->indv1;
        xv1 = iteratorEdges->xv1;
        yv1 = iteratorEdges->yv1;
        indv2 = iteratorEdges->indv2;
		xv2 = iteratorEdges->xv2;
		yv2 = iteratorEdges->yv2;
        inds1 = iteratorEdges->inds1;
        inds2 = iteratorEdges->inds2;

		iteratorEdges = iteratorEdges->next;

		return true;
	}


private:
	void cleanup();
	void cleanupEdges();
	char *getfree(struct Freelist *fl);	
	struct	Halfedge *PQfind();
	int PQempty();


	
	struct	Halfedge **ELhash;
	struct	Halfedge *HEcreate(), *ELleft(), *ELright(), *ELleftbnd();
	struct	Halfedge *HEcreate(struct Edge *e,int pm);


	struct Point PQ_min();
	struct Halfedge *PQextractmin();	
	void freeinit(struct Freelist *fl,int size);
	void makefree(struct Freenode *curr,struct Freelist *fl);
	void geominit();
	void plotinit();
	bool voronoi(int triangulate);
	void ref(struct Site *v);
	void deref(struct Site *v);
	void endpoint(struct Edge *e,int lr,struct Site * s);

	void ELdelete(struct Halfedge *he);
	struct Halfedge *ELleftbnd(struct Point *p);
	struct Halfedge *ELright(struct Halfedge *he);
	void makevertex(struct Site *v);
	void out_triple(struct Site *s1, struct Site *s2,struct Site * s3);

	void PQinsert(struct Halfedge *he,struct Site * v, double offset);
	void PQdelete(struct Halfedge *he);
	bool ELinitialize();
	void ELinsert(struct	Halfedge *lb, struct Halfedge *newHe);
	struct Halfedge * ELgethash(int b);
	struct Halfedge *ELleft(struct Halfedge *he);
	struct Site *leftreg(struct Halfedge *he);
	void out_site(struct Site *s);
	bool PQinitialize();
	int PQbucket(struct Halfedge *he);
	void clip_line(struct Edge *e);
	char *myalloc(unsigned n);
	int right_of(struct Halfedge *el,struct Point *p);

	struct Site *rightreg(struct Halfedge *he);
	struct Edge *bisect(struct	Site *s1,struct	Site *s2);
	double dist(struct Site *s,struct Site *t);
	struct Site *intersect(struct Halfedge *el1, struct Halfedge *el2, struct Point *p=0);

	void out_bisector(struct Edge *e);
	void out_ep(struct Edge *e);
	void out_vertex(struct Site *v);
	struct Site *nextone();

	void pushGraphEdge(int indv1, double x1, double y1, int indv2, double x2, double y2, int inds1, int inds2);

	void openpl();
	void line(int indv1, double xv1, double yv1, int indv2, double xv2, double yv2, int inds1, int inds2);
	void circle(double x, double y, double radius);
	void range(double minX, double minY, double maxX, double maxY);
                 
  double isLeft(struct vertice *P0, struct vertice *P1, struct vertice *P2);

	struct  Freelist	hfl;
	struct	Halfedge *ELleftend, *ELrightend;
	int 	ELhashsize;

	int		triangulate, sorted, plot, debug;
	double	xmin, xmax, ymin, ymax, deltax, deltay;

	struct	Site	*sites;
	int		nsites;
	int		siteidx;
	int		sqrt_nsites;
	int		nvertices;
	struct 	Freelist sfl;
	struct	Site	*bottomsite;

	int		nedges;
	struct	Freelist efl;
	int		PQhashsize;
	struct	Halfedge *PQhash;
	int		PQcount;
	int		PQmin;

	int		ntry, totalsearch;
	double	pxmin, pxmax, pymin, pymax, cradius;
	int		total_alloc;

	double borderMinX, borderMaxX, borderMinY, borderMaxY;

	FreeNodeArrayList* allMemoryList;
	FreeNodeArrayList* currentMemoryBlock;

	GraphEdge* allEdges;
	GraphEdge* iteratorEdges;

	double minDistanceBetweenSites;
	
};

int scomp(const void *p1,const void *p2);
int compara_vertices_orig(const void *a, const void *b);

#endif


