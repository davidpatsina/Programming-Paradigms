using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}


bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

// you should be implementing these two methods right here...

 bool imdb::getCredits(const string& player, vector<film>& films) const { 
	bool result = false;
	int actor_number = *(int*)actorFile;
	
	int low = 0;
	int high = actor_number;


	while (low <= high){
		int mid = (low + high)/2;
		int curr_actor_offset = *((int*)actorFile + mid);
		char* loc = (char*)actorFile + curr_actor_offset;
		string curr_act_name = loc;
		if (curr_act_name == player){
			int name_length = player.size();
			int walk = name_length;
			if (name_length % 2 == 0){
				walk += 2;
			}else {
				walk ++;
			}
			loc = loc + walk;
			short movie_number = *(short*)loc;
			if ((walk+2) % 4 != 0){
				loc += 2;
			
			}
			loc += 2;
			for (int j = 0; j < movie_number; j++){
				film f;
				int movie_offset = *((int*)loc + j);
				char* loc_in_movies = (char*)movieFile + movie_offset;	
				f.title = loc_in_movies;
				movie_offset += f.title.size() + 1;	
				f.year =  *((char*)movieFile+movie_offset) + 1900;
				films.push_back(f);
				
			}
			return true;
		} else if (curr_act_name > player){
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}
	return result;
}
bool imdb::getCast(const film& movie, vector<string>& players) const { 

	int movie_number =  *(int*)movieFile;
	string film_name = movie.title;
	
	int low = 0;
	int high = movie_number;
/*	
	while (low < high){
		int mid = (low + high)/2;
		int curr_movie_offset = *((int*)movieFile+mid);
		char* loc = (char*)movieFile+curr_movie_offset;
		string curr_name = loc;///
		int length_of_name = curr_name.size();
		int walk = length_of_name+1;	
		loc += walk;
		int curr_year = *(char*)loc + 1900;//
		walk++;
		loc += 1;
		if (walk % 2 == 1){
	       	walk++;
		loc += 1;
		}
	film f;
	f.title = curr_name;
	f.year = curr_year;
		
	if (f == movie){
		short actor_number = *(short*)loc;
			walk += 2;
			loc += 2;
			if (walk % 4 != 0) {
				walk += 2;
				loc += 2;
			}
			for (int j = 0; j < actor_number; j++){
				
				int actor_offset = *((int*)loc+j);
				string curr_actor_name = ((char*)actorFile + actor_offset);
				
				players.push_back(curr_actor_name);
			}
		return true;
		} else if (f < movie){
			
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}*/
	

	while (low <=  high){
		int mid = (low + high)/2;
		int curr_mov_offset = *((int*)movieFile + mid);
		char* loc = (char*)movieFile + curr_mov_offset;
		string curr_name = loc;///
		int length_of_name = curr_name.size();
		int walk = length_of_name+1;	
		loc += walk;
		int curr_year = *(char*)loc + 1900;//
		walk++;
		loc += 1;
	if (walk % 2 == 1){
	       	walk++;
		loc += 1;
	}	

	film f;
	f.title = curr_name;
	f.year = curr_year;
	if (movie == f){
		short actor_number = *(short*)loc;
			walk += 2;
			loc += 2;
			if (walk % 4 != 0) {
				walk += 2;
				loc += 2;
			}
			for (int j = 0; j < actor_number; j++){
				
				int actor_offset = *((int*)loc+j);
				string curr_actor_name = ((char*)actorFile + actor_offset);
				
				players.push_back(curr_actor_name);
			}
		return true;
		} else if (movie < f){
			high = mid -1;
		}else {
			low = mid + 1;
		}
	}

	return false;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAMs.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}