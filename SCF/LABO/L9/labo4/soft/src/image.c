#include <stdlib.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "image.h"

struct img_1D_t *load_image_1D(const char *path){
    struct img_1D_t *img;
	uint8_t *tmp;
	int i, comps, imgSize, iTimesComp;

    /* Allocate struct */
    img = (struct img_1D_t *) malloc(sizeof(struct img_1D_t));
    if ( ! img ) {
        fprintf(stderr, "[%s] struct allocation error\n", __func__);
        perror(__func__);
        return NULL;
    }

    /* Load the image */ 
    tmp = stbi_load(path, &(img->width), &(img->height), &(img->components), 0);
    if ( ! tmp ) {
		free(img);
        fprintf(stderr, "[%s] stbi load image failed\n", __func__);
        perror(__func__);
        return NULL;
    }

	comps = img->components;
	imgSize = img->width * img->height;

#ifdef DEBUG
    fprintf(stdout, "[%s] image %s loaded (%d components, %dx%d)\n", __func__,
            path, img->components, img->width, img->height);
#endif

	if (comps < COMPONENT_RGB) {
		if (comps == COMPONENT_GRAYSCALE) {
			img->r = tmp;
			/* Invalidate other pointer(s) */
			img->g = img->b = img->a = NULL;

		/* Split grouped Gray+Alpha array into different arrays */
		} else {
			img->r = (uint8_t *) malloc(imgSize);
			if ( ! (img->r) ) {
				free(tmp);
				free(img);
				fprintf(stderr, "[%s] RED allocation error\n", __func__);
				perror(__func__);
				return NULL;
			}

			img->a = (uint8_t *) malloc(imgSize);
			if ( ! (img->a) ) {
				free(img->r);
				free(tmp);
				free(img);
				fprintf(stderr, "[%s] ALPHA allocation error\n", __func__);
				perror(__func__);
				return NULL;
			}

			for (i = 0; i < imgSize; i++) {
				iTimesComp = i*comps;
				img->r[i] = tmp[iTimesComp];
				img->a[i] = tmp[iTimesComp+1];
			}
			/* Invalidate other pointer(s) */
			img->g = img->b = NULL;

			stbi_image_free(tmp);
		}

	/* Split a grouped RGB array into different arrays */
	} else if ((comps >= COMPONENT_RGB) && (comps <= COMPONENT_RGBA)) {
		img->r = (uint8_t *) malloc(imgSize);
		if ( ! (img->r) ) {
			free(tmp);
			free(img);
			fprintf(stderr, "[%s] RED allocation error\n", __func__);
			perror(__func__);
			return NULL;
		}

		img->g = (uint8_t *) malloc(imgSize);
		if ( ! (img->g) ) {
			free(img->r);
			free(tmp);
			free(img);
			fprintf(stderr, "[%s] GREEN allocation error\n", __func__);
			perror(__func__);
			return NULL;
		}

		img->b = (uint8_t *) malloc(imgSize);
		if ( ! (img->b) ) {
			free(img->g);
			free(img->r);
			free(tmp);
			free(img);
			fprintf(stderr, "[%s] BLUE allocation error\n", __func__);
			perror(__func__);
			return NULL;
		}

		if (comps == COMPONENT_RGB) {
			for (i = 0; i < imgSize; i++) {
				iTimesComp = i*comps;
				img->r[i] = tmp[iTimesComp + R_OFFSET];
				img->g[i] = tmp[iTimesComp + G_OFFSET];
				img->b[i] = tmp[iTimesComp + B_OFFSET];
			}
			/* Invalidate other pointer(s) */
			img->a = NULL;
		} else {
			img->a = (uint8_t *) malloc(imgSize);
			if ( ! (img->a) ) {
				free(img->b);
				free(img->g);
				free(img->r);
				free(tmp);
				free(img);
				fprintf(stderr, "[%s] ALPHA allocation error\n", __func__);
				perror(__func__);
				return NULL;
			}

			for (i = 0; i < imgSize; i++) {
				iTimesComp = i*comps;
				img->r[i] = tmp[iTimesComp + R_OFFSET];
				img->g[i] = tmp[iTimesComp + G_OFFSET];
				img->b[i] = tmp[iTimesComp + B_OFFSET];
				img->a[i] = tmp[iTimesComp + A_OFFSET];
			}
		}

	    stbi_image_free(tmp);
	}

    return img;
}

int save_image(const char *dest_path, const struct img_1D_t *img) {
    int ret;
	int i, comps, imgSize, iTimesComp;
    uint8_t *tmp;

	comps   = img->components;
	imgSize = img->width * img->height;
	if (comps < COMPONENT_RGB) {
		if (comps == COMPONENT_GRAYSCALE) {
			tmp = img->r;

		/* Split grouped Gray+Alpha array into different arrays */
		} else {
			tmp = (uint8_t *) malloc(img->width * img->height * comps);
			if ( ! tmp ) {
				fprintf(stderr, "[%s] image allocation error\n", __func__);
				perror(__func__);
				return 0;
			}

			for (i = 0; i < imgSize; i++) {
				iTimesComp = i*comps;
				tmp[iTimesComp]   = img->r[i];
				tmp[iTimesComp+1] = img->a[i];
			}
		}

	/* Regroup RGB into one array for the stb lib */
	} else if (comps <= COMPONENT_RGBA) {
		tmp = (uint8_t *) malloc(img->width * img->height * comps);
		if ( ! tmp ) {
			fprintf(stderr, "[%s] image allocation error\n", __func__);
			perror(__func__);
			return 0;
		}

		if (comps == COMPONENT_RGB) {
			for (i = 0; i < imgSize; i++) {
				iTimesComp = i*comps;
				tmp[iTimesComp + R_OFFSET] = img->r[i];
				tmp[iTimesComp + G_OFFSET] = img->g[i];
				tmp[iTimesComp + B_OFFSET] = img->b[i];
			}
		} else {
			for (i = 0; i < imgSize; i++) {
				iTimesComp = i*comps;
				tmp[iTimesComp + R_OFFSET] = img->r[i];
				tmp[iTimesComp + G_OFFSET] = img->g[i];
				tmp[iTimesComp + B_OFFSET] = img->b[i];
				tmp[iTimesComp + A_OFFSET] = img->a[i];
			}
		}
	}

    ret = stbi_write_png(dest_path, img->width, img->height, img->components, tmp, PNG_STRIDE_IN_BYTES);
    if(ret) {
        #ifdef DEBUG
        fprintf(stdout, "[%s] PNG file %s saved (%dx%d)\n", __func__, dest_path, img->width, img->height);
        #endif
    } else {
        fprintf(stderr, "[%s] save image failed\n", __func__);
    }

	if (tmp != img->r)	free(tmp);
    return ret;
}

struct img_1D_t *allocate_image_1D(int width, int height, int components) {
    struct img_1D_t *img;

    if (components == 0 || components > COMPONENT_RGBA)
        return NULL;

    /* Allocate struct */
    img = (struct img_1D_t *) malloc(sizeof(struct img_1D_t));
    if ( ! img ) {
        fprintf(stderr, "[%s] allocation error\n", __func__);
        perror(__func__);
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->components = components;

    /* Allocate space for image datas */
    img->r = (uint8_t *) calloc(img->width * img->height, sizeof(uint8_t));
    if ( ! (img->r) ) {
		free(img);
        fprintf(stderr, "[%s] RED allocation error\n", __func__);
        perror(__func__);
        return NULL;
    }

	if ((components >= COMPONENT_RGB) && (components <= COMPONENT_RGBA)) {
		img->g = (uint8_t *) calloc(img->width * img->height, sizeof(uint8_t));
		if ( ! (img->g) ) {
			free(img->r);
			free(img);
			fprintf(stderr, "[%s] GREEN allocation error\n", __func__);
			perror(__func__);
			return NULL;
		}

		img->b = (uint8_t *) calloc(img->width * img->height, sizeof(uint8_t));
		if ( ! (img->b) ) {
			free(img->g);
			free(img->r);
			free(img);
			fprintf(stderr, "[%s] BLUE allocation error\n", __func__);
			perror(__func__);
			return NULL;
		}
	} else {
		img->g = img->b = NULL;
	}

	if ((components == COMPONENT_GRAYALPHA) || (components == COMPONENT_RGBA)) {
		img->a = (uint8_t *) calloc(img->width * img->height, sizeof(uint8_t));
		if ( img->a ) return img;

		if ( img->g ) {
			free(img->b);
			free(img->g);
		}
		free(img->r);
		free(img);
		fprintf(stderr, "[%s] ALPHA allocation error\n", __func__);
		perror(__func__);
		return NULL;
	}

	return img;
}

void free_image(struct img_1D_t *img) {
	if (img->components == COMPONENT_GRAYSCALE) {
		stbi_image_free(img->r);
		free(img);
		return;
	}

	free(img->r);
	if ( img->g ) {
		free(img->g);
		free(img->b);
	}

	if ( img->a )	free(img->a);

	free(img);
}
