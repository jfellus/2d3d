/*
Copyright © CNRS 2012. 
Authors: David Picard, Philippe-Henri Gosselin, Romain Negrel, Hedi 
Tabia
Contact: picard@ensea.fr

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

*/
/**
 * \file Parameters.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Parameters_h__
#define __retin_Parameters_h__

#include "retin/toolbox/document/Parameter.h"

#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>

namespace retin
{

class	Parameters : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,Parameters);

	std::map<std::string,ParameterRef>	values;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Parameters& x)
	{
		s.object ("values",x.values);
	}

	bool 		isAlphaNum (char c);
	bool		isAlphaNum (const std::string& str);
	std::string 	nextWord (const std::string& str,size_t& i);


public:
	Parameters() { }
	Parameters(const std::string& params) { set(params); }
	Parameters(const Parameters& params);

	virtual bool	equals(const Document* doc) const;

	bool		has (const std::string& name) const
	{
		return values.find(name) != values.end();
	}

	template<class T>
	T		getAs (const std::string& name) const
	{
		std::map<std::string,ParameterRef>::const_iterator i = values.find(name);
		if (i == values.end())
			throw std::runtime_error("Le paramètre "+name+" n'existe pas");
		if (!i->second)
			throw std::runtime_error("Le paramètre "+name+" est vide");
		return boost::lexical_cast<T> ( i->second->value );
	}
	template<class T>
	T		getAs (const std::string& name,const T& defaultValue) const
	{
		std::map<std::string,ParameterRef>::const_iterator i = values.find(name);
		if (i == values.end())
			return defaultValue;
		if (!i->second)
			return defaultValue;
		return boost::lexical_cast<T> ( i->second->value );
	}

	template<class T>
	void	setAs (const std::string& name,T& value)
	{
		values[name] = boost::make_shared<Parameter> (boost::lexical_cast<std::string>(value));
	}

	std::string	get (const std::string& name) const { return getAs<std::string>(name); }
	std::string	get (const std::string& name,const std::string& defaultValue) const { return getAs<std::string>(name,defaultValue); }

	std::vector<std::string>	getNames() const;

	void	set (const std::string& params);
	void	set (const std::string& name,const std::string& value);
	void	set (const std::string& name,const std::string& value,const std::string& params);
	void	set (const std::string& name,const std::string& value,boost::shared_ptr<Parameters> params);

	void	setSubParams (const std::string& name,boost::shared_ptr<Parameters> subParams);
	bool	hasSubParams (const std::string& name) const;
	boost::shared_ptr<Parameters> getSubParams (const std::string& name) const;

	std::string		    	operator[] (const std::string& name) const { return get(name); }
	Parameters&		 	operator() (const std::string& name) const 
	{ 
		boost::shared_ptr<Parameters> params = getSubParams(name);
		if (!params) retinThrowException1("No parameter %s",name.c_str());
		return *params; 
	}
};

typedef boost::shared_ptr<Parameters> ParametersRef;

}

#endif
