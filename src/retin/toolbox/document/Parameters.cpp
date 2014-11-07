/**
 * \file Parameters.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/Parameters.h"

#include "retin/toolbox/document/serialization/Serializers.h" 
RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Parameters);

using namespace std;

namespace retin {


Parameters::Parameters(const Parameters& params)
{
    for (std::map<std::string,ParameterRef>::const_iterator ite=params.values.begin(),end=params.values.end();ite!=end;ite++) {
	values.insert(make_pair(ite->first,boost::make_shared<Parameter>(*ite->second)));
    }
}

bool	Parameters::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<Parameters>())
		return false; 
	const Parameters* x = static_cast<const Parameters*>(doc);
	if (x->values.size() != values.size())
		return false;
	for (map<string,ParameterRef>::const_iterator i=values.begin(),e=values.end();i!=e;i++)
	{
		map<string,ParameterRef>::const_iterator xi = x->values.find(i->first);
		if (xi == x->values.end())
			return false;
		if (!xi->second->equals(i->second.get()))
			return false;
	}
	return true;
}

std::vector<std::string>	Parameters::getNames() const
{
	vector<string> list;
	for (map<string,ParameterRef>::const_iterator i=values.begin(),e=values.end();i!=e;i++)
		list.push_back (i->first);
	return list;
}

void	Parameters::setSubParams (const std::string& name,boost::shared_ptr<Parameters> subParams)
{
	std::map<std::string,ParameterRef>::iterator i = values.find(name);
	if (i == values.end())
		throw std::runtime_error("Le paramètre "+name+" n'existe pas");
	if (!i->second)
		throw std::runtime_error("Le paramètre "+name+" est vide");
	i->second->params = subParams;
}
bool	Parameters::hasSubParams (const std::string& name) const
{
	std::map<std::string,ParameterRef >::const_iterator i = values.find(name);
	if (i == values.end())
		return false;
	return i->second->params;
}
boost::shared_ptr<Parameters> Parameters::getSubParams (const std::string& name) const
{
	std::map<std::string,ParameterRef >::const_iterator i;
	i = values.find(name);
	if (i == values.end())
		throw std::runtime_error ("Parameter "+name+" does not exists");
	if (!i->second->params)
		throw std::runtime_error ("Parameter "+name+" has no sub parameters");
	return i->second->params;
}

bool Parameters::isAlphaNum (char c)
{
	return (c != '=') && (c != '{') && (c != '}') && (c != ',');
}
bool Parameters::isAlphaNum (const std::string& str)
{
	if (str.empty())
		return false;
	for (size_t i=0;i<str.length();i++)
		if (!isAlphaNum(str[i]))
			return false;
	return true;
}

std::string Parameters::nextWord (const std::string& str,size_t& i)
{
	// Saute les espaces
	while (i<str.length() && str[i] <= 32) i++;
	if (i >= str.length())
		return "";

	char c = str[i];
	size_t i0 = i++;
	if (c == '"')
	{
		i0 ++;
		while (i<str.length() && str[i] != '"') i++;
		if (i >= str.length())
			retinThrowException("Symbole '\"' attendu");
		i ++;
		return str.substr(i0,i-i0-1);
	}
	else if (c == '\'')
	{
		i0 ++;
		while (i<str.length() && str[i] != '\'') i++;
		if (i >= str.length())
			retinThrowException("Symbole '\"' attendu");
		i ++;
		return str.substr(i0,i-i0-1);
	}
	else if (c == '{')
	{
		i0 ++;
		int level = 0;
		while (true)
		{
			string word = nextWord(str,i);
			if (word.empty() && i >= str.length()) {
				retinThrowException1("Symbole '}' attendu\n%s",str.c_str()+i);
			}
			else if (word == "}")
			{
				if (level == 0)
					break;
				level --;
			}
			else if (word == "{")
				level ++;
		}
		return str.substr(i0,i-i0-1);
	}
	else if (isAlphaNum(c))
	{
		while (i<str.length())
		{
			c = str[i];
			if (!isAlphaNum(c))
				break;
			i++;
		}
	}
	return str.substr(i0,i-i0);
}

void	Parameters::set (const std::string& name,const std::string& value)
{
	if (name.empty())
		retinThrowException ("Nom vide");
	if (value.empty())
		values[name] = boost::make_shared<Parameter> ();
	else
		values[name] = boost::make_shared<Parameter> (value);
}
void	Parameters::set (const std::string& name,const std::string& value,boost::shared_ptr<Parameters> params)
{
	if (name.empty())
		retinThrowException ("Nom vide");
	values[name] = boost::make_shared<Parameter> (value,params);
}
void	Parameters::set (const std::string& name,const std::string& value,const std::string& params)
{
	if (name.empty())
		retinThrowException ("Nom vide");
	if (params.empty())
		set (name,value);
	else
		set (name,value,boost::make_shared<Parameters>(params));
}
void	Parameters::set (const std::string& str)
{
	size_t i=0;
	while (true)
	{
		string name = nextWord(str,i);
		if (name.empty())
			break;
		if (name == ",")
			continue;
		string word = nextWord(str,i);
		if (word.empty() || word == ",")
		{
			set (name,"");
		}
		else if (word == "=")
		{
			string value = nextWord(str,i);
			if (value.empty())
				retinThrowException1("Valeur attendue après '%s='",name.c_str());
			word = nextWord(str,i);
			if (word.empty() || word == ",")
				set (name,value);
			else
				set (name,value,word);
		}
		else
		{
			set (name,"",word);
		}
	}
}

}
