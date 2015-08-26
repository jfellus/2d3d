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
#include "config_reader.h"


retin_config read_config_file(string file) {

	ifstream in;
	
	
	retin_config cf;
	
	in.open(file);
	char buf[4096];
	string key, value;
	while(in.good()) {
		in.getline(buf, 4096);
		if(buf[0] == '#')
			continue;
		istringstream iss(buf);
		
		getline(iss, key, '=');
		if(key.empty())
			continue;
		// trim key
		stringstream trimmer;
		trimmer << key;
		key.clear();
		trimmer >> key;
		
		// trim value
		getline(iss, value);
		trimmer << value;
		value.clear();
		trimmer >> value;
		if( key == "dict_size")
			cf.dict_size = atoi(value.c_str());
		if( key == "desc_scales") {
			cf.desc_scales.clear();
			istringstream siss(value);
			string s;
			do {
				s.clear();
				getline(siss, s, ',');
				if(!s.empty())
					cf.desc_scales.push_back(atoi(s.c_str()));
			} while(!s.empty());
		}
		if( key == "desc_step")
			cf.desc_step = atoi(value.c_str());
		if( key == "nb_views")
			cf.nb_views = atoi(value.c_str());
		if(key == "doc_list")
			cf.doc_list = value;
		if(key == "doc_dir")
			cf.doc_dir = value;
		if(key == "dict_dir")
			cf.dict_dir = value;
		if(key == "pre_pca")
			(value == "true")?cf.pre_pca=true:cf.pre_pca=false;
		if(key == "pre_pca_strategy")
			cf.pre_pca_strategy = value;
		if(key == "pre_pca_dim")
			cf.pre_pca_dim = atoi(value.c_str());
		if(key == "pre_pca_energy")
			cf.pre_pca_energy = atof(value.c_str());
		if(key == "vlat_file")
			cf.vlat_file = value;
		if(key == "power_norm")
			cf.power_norm = atof(value.c_str());
		if(key == "proj_dir")
			cf.proj_dir = value;
		if(key == "weight_file")
			cf.weight_file = value;
		if(key == "proj_file")
			cf.proj_file = value;
		if(key == "nb_proj")
			cf.nb_proj = atoi(value.c_str());
		if(key == "proj_white")
			(value=="true")?cf.proj_white=true:cf.proj_white=false;
		if(key == "compact_file")
			cf.compact_file = value;
		if(key == "feature_file")
			cf.feature_file = value;
		if(key == "matrix_file")
			cf.matrix_file = value;
		if(key == "desc_crop") {
			istringstream vs(value);
			vs >> cf.desc_minX;
			vs >> cf.desc_minY;
			vs >> cf.desc_maxX;
			vs >> cf.desc_maxY;
		}
		if(key == "desc_type")
			cf.desc_type = value;
		if(key == "proj_learning_set_fraction")
			cf.proj_learning_set_fraction = atoi(value.c_str());
		if(key == "vlat_inmemory")
			cf.vlat_inmemory = (value=="true");
		if(key == "release_vlat_after_compact")
			cf.release_vlat_after_compact = (value=="true");
		if(key == "thumbnails_dir")
			cf.thumbnails_dir = value;
		if(key=="similarity_score_sigmoid_factor")
			cf.similarity_score_sigmoid_factor = atof(value.c_str());
		if(key=="desc_output")
			cf.desc_output = value;
		if(key=="ndesc") {
			cf.ndesc = atoi(value.c_str());
		}
	}

	return cf;
}
