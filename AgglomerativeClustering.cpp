#include "AgglomerativeClustering.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

uint8_t* rgb_image;
int width, height, bpp;
int channelCount = 3;	

Cluster::Cluster(double* centers, int dim, int* point_indices, int cluster_size) {
	this->dim = dim;
	this->centers = new double[dim]();
	for(int i = 0; i < cluster_size; i++) {
		this->point_indices.push_back(point_indices[i]);
	}
	for(int i = 0; i < dim; i++) {
		this->centers[i] = centers[i];
	}
	this->cluster_size = cluster_size;
}
Cluster::Cluster() {

}
Cluster::~Cluster() {
	delete[] centers;
	centers = NULL;
	point_indices.clear();
}

void convertMatrixto_uint(double** im, int width, int height) {
	unsigned bytePerPixel = channelCount;
	unsigned char* pixelOffset = new unsigned char[3]();
	unsigned char r;
	unsigned char g;
	unsigned char b;
	uint8_t* rgb_image = new uint8_t[(width+height*height)*3]();
    // convert uint to an width*height by channelCount array
	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++) {
    		// access i jth pixel info
			pixelOffset[0] =(char) im[width*i+j][0];
			pixelOffset[1] =(char) im[width*i+j][1];
			pixelOffset[2] =(char) im[width*i+j][2];
			*(rgb_image + (i + height * j) * bytePerPixel) = pixelOffset[0];
			*(rgb_image + (i + height * j) * bytePerPixel + 1) = pixelOffset[1];
			*(rgb_image + (i + height * j) * bytePerPixel + 2) = pixelOffset[2];
		}
	} 
	stbi_write_jpg("result.jpg", width, height, 3, rgb_image, 100);
}

/*
 * Reads an image to uint8_t then converts it to double matrix with 3 channel
 *
 */
void readImage(char* filename, double** &im) {
    rgb_image = stbi_load(filename, &width, &height, &bpp, channelCount);
    
    unsigned bytePerPixel = channelCount;
	unsigned char r;
	unsigned char g;
	unsigned char b;

	im = new double*[width*height];
	for(int i = 0; i < width*height; i++)
    	im[i] = new double[channelCount];

    // convert uint to an width*height by channelCount array
	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++) {
    		// access i jth pixel info
			r = *(rgb_image + (i + height * j) * bytePerPixel);
			g = *(rgb_image + (i + height * j) * bytePerPixel+1);
			b = *(rgb_image + (i + height * j) * bytePerPixel+2);
			im[width*i+j][0] = (double)r;
			im[width*i+j][1] = (double)g;
			im[width*i+j][2] = (double)b;
		}
	} 
}

void destroyImage() {
	stbi_image_free(rgb_image);
}

double k_means_clustering(double** im, int dim, int k, int* &labels, double** &centers, double threshold, int max_iteration) {
	centers = new double*[k];
	int sampleSize = width*height;
	for(int i = 0; i < k; i++) 
    	centers[i] = new double[dim]();
   
	labels = new int[sampleSize]();
	default_random_engine generator;
	uniform_int_distribution<int> distribution(0,sampleSize/2);
	unordered_set<int> kcenters_index;

	// create a random k indices vector
	while(kcenters_index.size() < k) {
		kcenters_index.insert(distribution(generator)+distribution(generator)+1);		
	}
	// initialize cluster centers to randomly selected k sample points as the center seed
	int ind = 0;
	for ( auto it = kcenters_index.begin(); it != kcenters_index.end(); ++it ) {
		for(int j = 0; j < dim; j++) {
			centers[ind][j] = im[*it][j];
		}
		ind++;
	}
	double total_err = 100;
	double dist = 0;
	double min_dist = 0;
	double min_clus_ind = 0;
	int iteration = 0;
	int* cluster_sizes = new int[k]();
	for(int i = 0; i < k; i++) {
		cluster_sizes[i] = 1;
	}
	while(total_err >= threshold && iteration <= max_iteration) {
		for(int i = 0; i < sampleSize; i++) {
			// calculate distance to centroids
			min_dist = 0;
			for(int j = 0; j < dim; j++) {
				min_dist += abs(centers[0][j] - im[i][j]);
			}
			min_clus_ind = 0;
			for(int c = 0; c < k; c++) {
				dist = 0;
				for(int j = 0; j < dim; j++) {
					dist += abs(centers[c][j] - im[i][j]);
				}
				if(dist < min_dist) {
					min_dist = dist;
					min_clus_ind = c;
				}
			}
			// assign points to closest centers
			labels[i] = min_clus_ind;
			cluster_sizes[labels[i]]++;
		}

		// reassign cluster centroids
		for(int j = 0; j < dim; j++) {
			for(int i = 0; i < sampleSize; i++) {
				centers[labels[i]][j] += im[i][j];
			}
		}
		for(int c = 0; c < k; c++) {
			for(int j = 0; j < dim; j++) {
				centers[c][j] /= cluster_sizes[c];
			}
			// clear cluster sizes for the next iteration
			cluster_sizes[c] = 1;
		}

		// calculate total error, distance
		total_err = 0;
		for(int j = 0; j < dim; j++) {
			for(int i = 0; i < sampleSize; i++) {
					total_err += abs(centers[labels[i]][j] - im[i][j]);
				}
		}
		iteration++;
		// cout << "iteration:" << iteration << " total error:" << total_err <<endl; 
	}
	return total_err;
	delete[] cluster_sizes;

}

/*
 * Down scales image to size (width/radius,height/radius) based on box scaling method
 * by averaging pixels within box 
 */
void downScalebyAvg(double** &im, int dim, int radius, double** &scaledIm) {
	int sampleSize = width*height;
	double avg = 0;
	int new_size = (width/radius)*(height/radius);
	scaledIm  = new double*[new_size];
	for(int i = 0; i < new_size; i++) {
		scaledIm[i] = new double[dim]();
	}
	for(int d = 0;  d < dim; d++) {
		for(int i = 0; i < width-radius; i+=radius) {
			for(int j = 0; j < height-radius; j+=radius) {
				avg = 0;
				for(int k = 0; k < radius; k++) {
					for(int l = 0; l < radius; l++) {
						avg += im[i+k+width*(j+l)][d];
					}
				}
				avg /= (radius*radius);
				scaledIm[i/radius+(j/radius)*(width/radius)][d] = avg;
			}
		}
	}
}
/* 
 * Termination condition: stop when we have k clusters
 */
double agglomerative_hierarchical_clustering(double** &im, int dim, int k, int* &labels, double** &centers, int initialChoice, int initial_k) {
	
	vector<Cluster*> clusters;
	int sampleSize = width*height;
	centers = new double*[k];
	double** baseIm;
	double** scaledIm;
	int scaledsampleSize = sampleSize;
	for(int i = 0; i < k; i++) 
    	centers[i] = new double[dim]();


	// initialize clusters
    if(initialChoice == 0) { // each point is a cluster
    	baseIm = im;
		for(int i = 0; i < sampleSize; i++) {
			// each cluster contains only one point initially
			Cluster* newc = new Cluster( im[i], dim, &i, 1);
			clusters.push_back(newc);
		}
    }
    else if(initialChoice == 1) { // kmeans determine initial clusters

    	baseIm = im;
    	double** centersx;
    	int* labelsx;
		k_means_clustering(im, dim, initial_k , labelsx, centersx, 0.1,10); 
		for(int i = 0; i < initial_k; i++) {
			Cluster* newc = new Cluster();
			newc->centers = new double[dim](); 
			newc->dim = dim;
			for(int d = 0; d < dim; d++) {
				newc->centers[d] = centersx[i][d];
			}
			clusters.push_back(newc);
		}
		for(int i = 0; i < sampleSize; i++) {
			clusters[labelsx[i]]->point_indices.push_back(i);
		}
		for(int i = 0; i < initial_k; i++) {
			clusters[i]->cluster_size = clusters[i]->point_indices.size();
		}
		delete[] labelsx;
		free2D(centersx, initial_k);
    } 
    else { // downsample the image
    	int radius = 5;
    	downScalebyAvg(im, dim, radius, scaledIm);
    	baseIm = scaledIm;
    	scaledsampleSize = (width/radius)*(height/radius);
    	for(int i = 0; i < scaledsampleSize; i++) {
			// each cluster contains only one point initially
			Cluster* newc = new Cluster( baseIm[i], dim, &i, 1);
			clusters.push_back(newc);
		}
    }

	labels = new int[scaledsampleSize]();
	int min_clus_ind = 0;
	int min_dist = 0;
	int dist = 0;
	int i = 0;
	int j = 0;
	while(clusters.size() > k) {
		// compute pairwise distances between all pairs of clusters // single linkage??
		i = 0;
		while(i < clusters.size()) {
			
			j = 0;
			min_clus_ind = 0;
			min_dist = 1000;
			for(int d = 0; d < dim; d++)
				min_dist += abs(clusters[0]->centers[d] - clusters[i]->centers[d]);
			while(j < clusters.size()) {
				if(i == j) {
					j++;
					continue;
				}
				dist = 0;
				for(int d = 0; d < dim; d++) {
					dist += abs(clusters[i]->centers[d] - clusters[j]->centers[d]);
				}

				if(dist < min_dist) {
					min_dist = dist;
					min_clus_ind = j;
				}
				j++;			
			}

			// merge the clusters c_i c_j into a new cluster c_i+j by updating c_i
			for(int d = 0; d < dim; d++) {
				clusters[i]->centers[d] = (clusters[i]->centers[d]*clusters[i]->cluster_size+clusters[min_clus_ind]->centers[d]*clusters[min_clus_ind]->cluster_size)/(clusters[i]->cluster_size+clusters[min_clus_ind]->cluster_size);
			}
			for(int l = 0; l < clusters[min_clus_ind]->cluster_size; l++) {
				clusters[i]->point_indices.push_back(clusters[min_clus_ind]->point_indices[l]);
			}
			clusters[i]->cluster_size += clusters[min_clus_ind]->cluster_size;
			clusters[min_clus_ind]->point_indices.clear();
			clusters[min_clus_ind]->cluster_size = 0;
			// remove c_j from the collection C 
			delete clusters[min_clus_ind];
			clusters[min_clus_ind] = NULL;
			clusters.erase(clusters.begin()+min_clus_ind);	
			if(min_clus_ind >= i)
				i++;
			if(clusters.size() <= k)
				break;
		}
	}
	double err = 0;
	for(int i = 0; i < clusters.size(); i++) {
		for(int d = 0; d < dim; d++) {
			centers[i][d] = clusters[i]->centers[d];
		}
		for(int j = 0; j < clusters[i]->cluster_size; j++) {
			for(int d = 0; d < dim; d++) {
				err += abs(clusters[i]->centers[d] - baseIm[clusters[i]->point_indices[j]][d]);
			}
			labels[clusters[i]->point_indices[j]] = i; 
		}

		delete clusters[i];
		clusters[i] = NULL;
	}
	free2D(baseIm, scaledsampleSize);
	clusters.clear();
	return err;
}

void free2D(double** &arr,int row) {
	for(int j = 0; j < row; j++) {
		delete[] arr[j];
	}	
	delete[] arr;
	arr = NULL;
}

bool writeData(const char* filename_labels, int* &labels, const char* filename_centers, double** &centers, int dim, int sampleSize, int k) {
	ofstream xfile(filename_centers);
	if (xfile.is_open())
	{
		for(int i = 0; i < k; i++) {
			for(int j = 0; j < dim -1; j++) {
				xfile << centers[i][j] << ",";
			}
			xfile << centers[i][dim-1];
			xfile << "\n"; 
		}
		xfile.close();
	}
	else return -1;

	xfile.open(filename_labels);
	if (xfile.is_open())
	{
		for(int i = 0; i < sampleSize; i++) {
			xfile << labels[i];
			xfile << "\n"; 
		}
		xfile.close();
	}
	else return -1;
} 