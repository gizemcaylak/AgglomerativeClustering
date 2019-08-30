#include "AgglomerativeClustering.h"

int main() {
    
    double** im;
    readImage((char *)"sample.jpg", im);

	int* labels;
	double** centers;
	double threshold = 0.1;
	int k_s[7] = {2, 4, 8, 16, 32, 64, 128};
	int sampleSize = height*width;
	int max_iteration = 100;
	int error = 0;
	for(int k = 0; k < 7; k++) {
		error = k_means_clustering(im, 3,  k_s[k], labels, centers, threshold, max_iteration);    
		cout << "Centers obtained via k-means clustering with k = " <<  k_s[k] <<endl;

		for(int j = 0; j < k_s[k]; j++) {
			for(int i = 0; i < 3; i++) {
				cout << centers[j][i] << " ";
			} 
			cout << endl;
		}
		cout << "K-means clustering error:" << error << endl;

		// write centers and labels
		writeData(("kmeans_labels_k_"+to_string(k_s[k])+".csv").c_str(), labels, ("kmeans_centers_k_"+to_string(k_s[k])+".csv").c_str(), centers, channelCount, sampleSize, k_s[k]);
		free2D(centers,k_s[k]);
		delete[] labels;
		labels = NULL;
		error = agglomerative_hierarchical_clustering(im, 3,  k_s[k], labels, centers,2,300);
	    cout << "Centers obtained via agglomerative hierarchical clustering with k = " <<  k_s[k] <<endl;
	    for(int j = 0; j < k_s[k]; j++) {
			for(int i = 0; i < 3; i++) {
				cout << centers[j][i] << " ";
			} 
			cout << endl;
		}
		cout << "Agglomerative clustering error:" << error << endl;

		// write centers and labels
		writeData(("ahc_labels_k_"+to_string(k_s[k])+".csv").c_str(), labels, ("ahc_centers_k_"+to_string(k_s[k])+".csv").c_str(), centers, channelCount, 14400, k_s[k]);


		free2D(centers, k_s[k]);
		delete[] labels;
		labels = NULL;
	}
	
	free2D(im, height*width);
	destroyImage();
    return 0;
}