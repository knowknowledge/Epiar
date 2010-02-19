/**\file			models.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/models.h"
#include "Utilities/parser.h"

/**\class Models
 * \brief Handles ship models. */

struct model_name_equals
	: public std::binary_function<Model*, string, bool>
{
	bool operator()( Model* model, const string& name ) const
	{
		return model->GetName() == name;		
	}
};

Model::Model() {
}

Model::Model(const Model& other) {
	name = other.name;
	image = other.image;
	engine = other.engine;
	mass = other.mass;
	thrustOffset = other.thrustOffset;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
}

Model::Model( string _name, Image* _image, Engine* _engine, float _mass, short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _maxEnergyAbsorption) :
	name(_name),
	image(_image),
	engine(_engine),
	mass(_mass),
	thrustOffset(_thrustOffset),
	rotPerSecond(_rotPerSecond),
	maxSpeed(_maxSpeed)
{}


Models *Models::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Model instance.
 * \return Pointer to the Model instance
 */
Models *Models::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Models; // create the sold instance
	}
	return( pInstance );
}

/**\brief Loads a model from an XML file
 * \param filename String containing filename
 */
bool Models::Load( string& filename )
{
	Parser<Model> parser;
	
	models = parser.Parse( filename, "models", "model" );

	for( list<Model *>::iterator i = models.begin(); i != models.end(); ++i ) {
		(*i)->_dbg_PrintInfo();
	}

	return true;
}

bool Models::Save( string filename )
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, section = NULL;/* node pointers */
    char buff[256];

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "models");
    xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, BAD_CAST "version-major", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "version-minor", BAD_CAST "7");
	xmlNewChild(root_node, NULL, BAD_CAST "version-macro", BAD_CAST "0");

	for( list<Model *>::iterator i = models.begin(); i != models.end(); ++i ) {
		section = xmlNewNode(NULL, BAD_CAST "model");
		xmlAddChild(root_node, section);
		
		xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST (*i)->GetName().c_str() );
		xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST (*i)->GetImage()->GetPath().c_str() );
		xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST (*i)->GetEngine()->GetName().c_str() );
        sprintf(buff, "%1.2f", (*i)->GetMass() );
		xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );
        sprintf(buff, "%1.2f", (*i)->GetRotationsPerSecond() );
		xmlNewChild(section, NULL, BAD_CAST "rotationsPerSecond", BAD_CAST buff );
        sprintf(buff, "%d", (*i)->GetThrustOffset() );
		xmlNewChild(section, NULL, BAD_CAST "thrustOffset", BAD_CAST buff );
        sprintf(buff, "%1.1f", (*i)->GetMaxSpeed() );
		xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );
        sprintf(buff, "%d", (*i)->getMaxEnergyAbsorption() );
		xmlNewChild(section, NULL, BAD_CAST "maxEnergyAbsorption", BAD_CAST buff );
	}

	xmlSaveFormatFileEnc( filename.c_str(), doc, "ISO-8859-1", 1);
	return true;
}

/**\brief Returns the specified Model.
 * \param modelName Name of the model to get.
 * \return Pointer to the Model
 */
Model * Models::GetModel( string& modelName )
{
	list<Model *>::iterator i = std::find_if( models.begin(), models.end(), std::bind2nd( model_name_equals(), modelName ));
	if( i != models.end() ) {
		return( *i );
	}
	cout<<"Couldn't find the Model Named: "<<modelName<<endl;
	return( NULL );
}

/**\brief Returns a list of all the Model names
 * \return list of Model names.
 */
list<string>* Models::GetModelNames()
{
	list<string> *names = new list<string>();
	for( list<Model *>::iterator i = models.begin(); i != models.end(); ++i ) {
		names->push_back( (*i)->GetName() );
	}
	return names;
}
