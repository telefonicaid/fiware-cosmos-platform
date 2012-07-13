/*
 *  CliquesSolver.h
 *  cdr_filter
 *
 *  Created by ANDREU URRUELA PLANAS on 10/21/09.
 *  Copyright 2009 TID. All rights reserved.
 *
 */

#ifndef _H_CliqueSolution
#define _H_CliqueSolution


#include <algorithm>            // std::sort
#include <sstream>
#include <string>				// std::string
#include <iostream>              // std::err

#include <stdio.h>
#include <string.h>
#include <samson/module/Log.h>




#define MAX_NODES		  500
#define MAX_SOLUTIONS	 5000


/**
 \page page_Solving_Cliques Solving Cliques
 
 One of the most common in SNA (Social Network Analysis) is the cliques finding.
 IN MACRO Platform, we have implemented  generic class to solve this problem in a general way.
 
 See specific documentation at \ref CliquesSolver
 
 */


class CliqueSolution
{

public:

	int nodes[MAX_NODES];	
	int num_nodes;
	
	int pos;	//Position flag to explore possibilities
	
	CliqueSolution()
	{
		num_nodes=0;
	}
	
	void clear()
	{
		num_nodes=0;
	}
	
	void add(int node)
	{
		nodes[num_nodes++] = node;
	}
	
	void setDefaultSolution( int _num_nodes ) 
	{
		if ( _num_nodes > MAX_NODES )
		{
			//OLM_E(("Error: ( _num_nodes(%d) > MAX_NODES(%d) )", _num_nodes, MAX_NODES));
			exit(1);
		}
		num_nodes = _num_nodes;
		for (int i = 0  ; i < num_nodes ; i++)
			nodes[i] = i;
		pos = -1;
	
	}

	
	int getPreviousSelectedNode()
	{
		if (pos <= 0)
		{
			//OLM_E(("Error: (pos(%d) <= 0)", pos));
			exit(1);
		}

		if (pos > num_nodes)
		{
			//OLM_E(("Error: (pos(%d) > num_nodes(%d))", pos, num_nodes));
			exit(1);
		}
		return nodes[ pos -1 ];
		/*Note: This is necessary because "pos" is always pointing to the next element*/
	}
	
	std::string toString()
	{
		std::ostringstream outputStream;
		outputStream << "[";
		for (int i = 0 ;i < num_nodes ; i++)
		{
			outputStream << nodes[i];
			if (i!=(num_nodes-1))
				outputStream<<",";
		}
		outputStream << "]";
		return outputStream.str();
	}
	
	
	void setFrom( CliqueSolution *other )
	{
		num_nodes = other->num_nodes;
		if ( num_nodes > MAX_NODES )
		{
			//OLM_E(("Error: ( _num_nodes(%d) > MAX_NODES(%d) )", num_nodes, MAX_NODES));
			exit(1);
		}
		for (int i = 0 ; i < other->num_nodes ; i++)
			nodes[i] = other->nodes[i];
	}

	
	void addAll( CliqueSolution *other )
	{
		for (int i = 0 ; i < other->num_nodes ; i++)
			nodes[num_nodes+i] = other->nodes[i];
		
		num_nodes += other->num_nodes;
		if ( num_nodes > MAX_NODES )
		{
			//OLM_E(("Error: ( _num_nodes(%d) > MAX_NODES(%d) )", num_nodes, MAX_NODES));
			exit(1);
		}
	}

	bool isEqual( CliqueSolution * other )
	{
		if ( num_nodes != other->num_nodes)
			return false;
		
		for (int i = 0 ; i < num_nodes ; i++)
			if ( nodes[i] != other->nodes[i] )
				return false;
		
		return true;
	}
	
	void sort()
	{
		std::sort( nodes, nodes+num_nodes );
	}
	
	
} ;



#define LENGTH(v)	sizeof(v)/sizeof(v[0])

class CliquesSolver;


class CliquesSolverLevelInfo
{

	
public:
	CliquesSolver *solver;	// Pointer to the solver to access connection matrix
	
	CliqueSolution nodes;			//Considered node at this level
	CliqueSolution perfectNodes;	//List of perfecto nodes ( Nodes that see all the rest of elements)
	
	void removePerfectoNodes( );
	
} ;


/**
 
 Class to solve the cliques problem.
 
 Given a set of nodes possibliy connected amoung them, find the maximum groups of nodes
 where all the nodes are connected to all of them.
 Note that a node can be included in multiple cliques.
 
 Taking into accout that a node can be whatever, this implementation is independent of the real elements,
 we are just interested in the connection amoung nodes.
 
 All the connections are included in the connections matrix "connections" either directly or using appropiate
 methods like 
 
 - setConnection
 - setConnectionOfNode
 
 Note that if this element is reused to compute cliques again, the method clearConnectionsMatrix(int num_nodes)
 should be called indicating the number of nodes considered in the next usage
 
 Once all connections are set, we call the "solveCliques" to compute everything.
 
 Results can be retreived with fields:
 
 CliqueSolution *solutions;
 int num_solutions;

 */


class CliquesSolver
{

	
public:

	int min_nodes_in_solution;	//By default is 3, it could be changed to "2" or "1"
	
	//Information about all nodes (necessary to find cliques)
	bool connections[MAX_NODES][MAX_NODES];	// Connections matrix (visibility matrix between considered nodes)	
	int num_nodes;

	
	//Nodes considered at each level
	CliquesSolverLevelInfo *infoAtLevel;
	
	//Output with the cliques
	CliqueSolution *solutions;
	int num_solutions;
	int max_solutions;	//Space allocated for solutions
	

public:

	CliquesSolver( )
	{
		min_nodes_in_solution = 2;	//Default minimum size of the clique (to find cliques of size 3)
		
		solutions		= new CliqueSolution[MAX_SOLUTIONS]; 
		max_solutions = MAX_SOLUTIONS;
		infoAtLevel		= new CliquesSolverLevelInfo[MAX_NODES];  
		
		for (int i = 0 ; i < MAX_NODES ; i++) 
			infoAtLevel[i].solver = this;
	};
	
	~CliquesSolver()
	{
		delete[] solutions;
		delete[] infoAtLevel;
	}

	//Function to compute cliques
	void clearConnectionsMatrix( int _num_nodes )
	{
		num_nodes = _num_nodes;
		
		if ( num_nodes < 0 )
		{
			//OLM_E(("Error: ( num_nodes(%d) < 0 )", num_nodes));
			exit(1);
		}
		if ( num_nodes > MAX_NODES )
		{
			//OLM_E(("Error: ( _num_nodes(%d) > MAX_NODES(%d) )", num_nodes, MAX_NODES));
			exit(1);
		}
		
		for (int i = 0 ; i < num_nodes ; i++)
			for (int j = 0 ; j < num_nodes ; j++)
				connections[i][j] = false;
		
	}
	
	void setConnection( int i , int j)
	{
		connections[i][j] = true;
		connections[j][i] = true;
	}
	
	void setConnectionOfNode( int i, int *connections , int nun_connections_of_this_node )
	{
		for (int j = 0 ; j < nun_connections_of_this_node ; j++)
			setConnection(i, connections[j] );
	}
	
	void solveCliques( );	//Previously connections matrix should be filled
	

	void printIndent( int level)
	{
		for (int i = 0 ; i < level ; i++) fprintf(stderr, " ");
	}
	
	void printStack(int level)
	{

		printIndent(level);
		fprintf(stderr, "Level %d\n", level);
		for (int i = 0 ; i < level ; i++)
		{
			
			printIndent(level);
			fprintf(stderr, "\tlevel %d P nodes: %s\n", i , infoAtLevel[i].perfectNodes.toString().c_str() );

			printIndent(level);
			fprintf(stderr, "\tlevel %d N nodes:[%d/%d] %s\n", i ,infoAtLevel[i].nodes.pos , infoAtLevel[i].nodes.num_nodes, infoAtLevel[i].nodes.toString().c_str() );
		}
	}
	
	void printConnectionsMatrix()
	{
	
		fprintf(stderr, "Connection matrix: %d\n", num_nodes);
		fprintf(stderr, "--------------------------------------------\n");
		for (int i = 0 ; i < num_nodes ; i++)
		{
			for (int j = 0 ; j < num_nodes ; j++)
				fprintf(stderr, "%c ", connections[i][j]?'X':' ' );
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "--------------------------------------------\n");
	}
	
	static void test()
	{
		int nodes_0[] = {1,2};
		int nodes_1[] = {0,2};
		int nodes_2[] = {0,1,3,4,5};
		int nodes_3[] = {2,4,5};
		int nodes_4[] = {2,3,5};
		int nodes_5[] = {2,3,4};

		fprintf(stderr, "Test with 6 nodes\n");
		
		CliquesSolver solver;
		solver.clearConnectionsMatrix(6);
		
		solver.setConnectionOfNode(0, nodes_0 , LENGTH(nodes_0));
		solver.setConnectionOfNode(1, nodes_1 , LENGTH(nodes_1));
		solver.setConnectionOfNode(2, nodes_2 , LENGTH(nodes_2));
		solver.setConnectionOfNode(3, nodes_3 , LENGTH(nodes_3));
		solver.setConnectionOfNode(4, nodes_4 , LENGTH(nodes_4));
		solver.setConnectionOfNode(5, nodes_5 , LENGTH(nodes_5));
		
		solver.solveCliques();
		
		for (int i = 0 ; i < solver.num_solutions ; i++)
			fprintf(stderr, "Solution %d: %s\n", i , solver.solutions[i].toString().c_str() );
		 
	}
};

#endif



