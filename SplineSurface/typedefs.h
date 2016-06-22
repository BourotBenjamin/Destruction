#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3_to_polyhedron_3.h>
#include <CGAL/algorithm.h>
#include <list>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Nef_polyhedron_3.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel     K;
typedef CGAL::Delaunay_triangulation_3<K>                       DT3;
typedef DT3::Vertex_handle										Vertex_handle;
typedef CGAL::Polyhedron_3<K>                                   Polyhedron_3;


typedef CGAL::Nef_polyhedron_3<K>  Nef_polyhedron;
#endif
