#include "GenomeUtil.h"

#include <assert.h>

#include "agent.h"
#include "Brain.h"
#include "Gene.h"
#include "GenomeLayout.h"
#include "globals.h"
#include "GroupsGenome.h"
#include "GroupsGenomeSchema.h"
#include "Metabolism.h"
#include "misc.h"

using namespace std;
using namespace genome;

GenomeSchema *GenomeUtil::schema = NULL;
GenomeLayout *GenomeUtil::layout = NULL;


GenomeSchema *GenomeUtil::createSchema()
{
	assert(schema == NULL);

	// ---
	// --- Schema
	// ---
	schema = new GroupsGenomeSchema();
	schema->defineImmutable();
	schema->defineMutable();
	schema->complete();	

	// ---
	// --- Configure Interpolation
	// ---
	itfor( GenomeSchema::Configuration::GeneInterpolationPowers, GenomeSchema::config.geneInterpolationPower, it )
	{
		Gene *gene = schema->get( it->first );
		if( !gene )
		{
			cerr << "Invalid gene name for interpolation power: " << it->first << endl;
			exit( 1 );
		}

		__InterpolatedGene *igene = GeneType::to___Interpolated( gene );
		if( !igene )
		{
			cerr << "Invalid gene for interpolation power: " << it->first << endl;
			exit( 1 );
		}
		
		igene->setInterpolationPower( it->second );
	}

	// ---
	// --- Layout
	// ---
	layout = GenomeLayout::create( schema, GenomeSchema::config.layoutType );

	return schema;
}

Genome *GenomeUtil::createGenome( bool randomized )
{
	assert(schema);
	assert(layout);

	Genome *g = new GroupsGenome( dynamic_cast<GroupsGenomeSchema *>(schema), layout );

	if( randomized )
	{
		randomize( g );
	}
	
	return g;
}

void GenomeUtil::randomize( Genome *g )
{
#if DesignerGenes
	seed( g );
#else
	g->randomize();
#endif
}

void GenomeUtil::seed( Genome *g )
{
	assert( schema );
	assert( g );

	schema->seed( g );
}

const Metabolism *GenomeUtil::getMetabolism( Genome *g )
{
	int index;

	if( Metabolism::getNumberOfDefinitions() == 1 )
	{
		index = 0;
	}
	else
	{
		index = (int)g->get( "MetabolismIndex" );
	}

	return Metabolism::get( index );
}

Gene *GenomeUtil::getGene( const std::string &name, const std::string &err )
{
	Gene *gene = schema->get( name );
	if( !gene && !err.empty() )
	{
		cerr << err << endl;
		exit( 1 );
	}
	return gene;
}
