#ifndef CLUSTERING
  #define CLUSTERING

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <random>
#include <iterator>
#include <math.h>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <utility>
#include <fstream>  
#include <unordered_set>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm> // random_shuffle
using namespace std;

extern uint8_t* rgb_image;
extern int width, height, bpp;
extern int channelCount;	


/* Structure for the agglomerative hierarchical clustering */
struct Cluster
{
	double* centers = NULL;		// = new double[dim]();
	vector<int> point_indices;	// indices of image pixels within cluster
	int dim = 0;
	double cluster_size = 0;	// number of points within cluster
	Cluster();
	Cluster(double* centers, int dim, int* point_indices, int cluster_size);
	~Cluster();
};


void convertMatrixto_uint(double** im, int width, int height);
void readImage(char* filename, double** &im);
void downScalebyAvg(double** &im, int dim, int radius, double** &scaledIm);
double k_means_clustering(double** im, int dim, int k, int* &labels, double** &centers, double threshold, int max_iteration);
double agglomerative_hierarchical_clustering(double** &im, int dim, int k, int* &labels, double** &centers, int initialChoice, int initial_k);
void free2D(double** &arr,int row);
void destroyImage();
bool writeData(const char* filename_labels, int* &labels, const char* filename_centers, double** &centers, int dim, int sampleSize, int k);
#endif