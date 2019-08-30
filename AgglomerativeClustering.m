function AgglomerativeClustering(input_image, labels_filename, centers_filename, clustered_im_out_filename) 
% @author: Gizem Caylak
% Reads calculated (in C++ files) centers and labels vectors and, 
% obtains the clustered image and, 
% writes it to a file (clustered_im_out_filename)


% Retrieve information about the image
info = imfinfo(input_image);

% RUN C++ files 

% Read C++ clustering results

centers = csvread(centers_filename);
labels = csvread(labels_filename);

cmap2 = reshape(labels,[info.Height info.Width]);
M = centers / 255;
clusteredImage = label2rgb(cmap2, M);
% Shows the clustered image in Matlab and writes it into a bitmap file
figure, imshow(clusteredImage)
imwrite(clusteredImage, clustered_im_out_filename)

end % AgglomerativeClustering create clustered image
