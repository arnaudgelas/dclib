// Copyright (C) 2008  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <dlib/pixel.h>
#include <dlib/array2d.h>
#include <dlib/image_transforms.h>
#include <dlib/image_io.h>
#include <dlib/matrix.h>
#include <dlib/rand.h>

#include "tester.h"

namespace  
{
    using namespace test;
    using namespace dlib;
    using namespace std;

    logger dlog("test.image");


    void image_test (
    )
    /*!
        ensures
            - runs tests on pixel objects and functions for compliance with the specs 
    !*/
    {        

        print_spinner();

        array2d<unsigned char> img1, img2;

        img1.set_size(100,100);

        assign_all_pixels(img1,7);

        assign_image(img2, img1);

        DLIB_TEST_MSG(img1.nr() == 100 && img1.nc() == 100 &&
                     img2.nr() == 100 && img2.nc() == 100,"");


        for (long r = 0; r < img1.nr(); ++r)
        {
            for (long c = 0; c < img1.nc(); ++c)
            {
                DLIB_TEST(img1[r][c] == 7);
                DLIB_TEST(img2[r][c] == 7);
            }
        }

        img2.clear();
        DLIB_TEST(img2.size() == 0);
        DLIB_TEST(img2.nr() == 0);
        DLIB_TEST(img2.nc() == 0);
        assign_image(img2, array_to_matrix(img1));

        DLIB_TEST_MSG(img1.nr() == 100 && img1.nc() == 100 &&
                     img2.nr() == 100 && img2.nc() == 100,"");


        for (long r = 0; r < img1.nr(); ++r)
        {
            for (long c = 0; c < img1.nc(); ++c)
            {
                DLIB_TEST(img1[r][c] == 7);
                DLIB_TEST(img2[r][c] == 7);
            }
        }


        threshold_image(img1, img2, 4);

        for (long r = 0; r < img1.nr(); ++r)
        {
            for (long c = 0; c < img1.nc(); ++c)
            {
                DLIB_TEST(img1[r][c] == 7);
                DLIB_TEST(img2[r][c] == on_pixel);
            }
        }

        {
            array2d<hsi_pixel> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].h = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].s = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].i = static_cast<unsigned char>(r*14 + c + 3);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            istringstream sin(sout.str());

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_dng(img, sin);
            
            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c].h == r*14 + c + 1);
                    DLIB_TEST(img[r][c].s == r*14 + c + 2);
                    DLIB_TEST(img[r][c].i == r*14 + c + 3);
                }
            }
        }




        {
            array2d<rgb_alpha_pixel> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].red = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].green = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].blue = static_cast<unsigned char>(r*14 + c + 3);
                    img[r][c].alpha = static_cast<unsigned char>(r*14 + c + 4);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            istringstream sin(sout.str());

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_dng(img, sin);
            
            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c].red == r*14 + c + 1);
                    DLIB_TEST(img[r][c].green == r*14 + c + 2);
                    DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                    DLIB_TEST(img[r][c].alpha == r*14 + c + 4);
                }
            }
        }

#ifdef DLIB_PNG_SUPPORT
        {
            array2d<rgb_alpha_pixel> img;
            array2d<rgb_pixel> img2, img3;
            img.set_size(14,15);
            img2.set_size(img.nr(),img.nc());
            img3.set_size(img.nr(),img.nc());
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].red = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].green = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].blue = static_cast<unsigned char>(r*14 + c + 3);
                    img[r][c].alpha = static_cast<unsigned char>(r*14 + c + 4);
                }
            }

            save_png(img, "test.png");

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_png(img, "test.png");
            
            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            assign_all_pixels(img2, 255);
            assign_all_pixels(img3, 0);
            load_png(img2, "test.png");
            assign_image(img3, img);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c].red == r*14 + c + 1);
                    DLIB_TEST(img[r][c].green == r*14 + c + 2);
                    DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                    DLIB_TEST(img[r][c].alpha == r*14 + c + 4);

                    DLIB_TEST(img2[r][c].red == img3[r][c].red);
                    DLIB_TEST(img2[r][c].green == img3[r][c].green);
                    DLIB_TEST(img2[r][c].blue == img3[r][c].blue);
                }
            }
        }
#endif // DLIB_PNG_SUPPORT



        {
            array2d<rgb_pixel> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].red = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].green = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].blue = static_cast<unsigned char>(r*14 + c + 3);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            save_bmp(img, sout);
            save_dng(img, sout);
            save_bmp(img, sout);
            istringstream sin(sout.str());

            for (int i  = 0; i < 2; ++i)
            {
                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_dng(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST(img[r][c].red == r*14 + c + 1);
                        DLIB_TEST(img[r][c].green == r*14 + c + 2);
                        DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                    }
                }

                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_bmp(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST_MSG(img[r][c].red == r*14 + c + 1, "got " << (int)img[r][c].red << "  but expected " << r*14 + c + 1);
                        DLIB_TEST(img[r][c].green == r*14 + c + 2);
                        DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                    }
                }
            }
        }
        {
            array2d<bgr_pixel> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].red = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].green = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].blue = static_cast<unsigned char>(r*14 + c + 3);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            save_bmp(img, sout);
            save_dng(img, sout);
            save_bmp(img, sout);
            istringstream sin(sout.str());

            for (int i  = 0; i < 2; ++i)
            {
                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_dng(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST(img[r][c].red == r*14 + c + 1);
                        DLIB_TEST(img[r][c].green == r*14 + c + 2);
                        DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                    }
                }

                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_bmp(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST_MSG(img[r][c].red == r*14 + c + 1, "got " << (int)img[r][c].red << "  but expected " << r*14 + c + 1);
                        DLIB_TEST(img[r][c].green == r*14 + c + 2);
                        DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                    }
                }
            }
        }

#ifdef DLIB_PNG_SUPPORT
        {
            array2d<rgb_pixel> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].red = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].green = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].blue = static_cast<unsigned char>(r*14 + c + 3);
                }
            }

            save_png(img, "test.png");

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_png(img, "test.png");

            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c].red == r*14 + c + 1);
                    DLIB_TEST(img[r][c].green == r*14 + c + 2);
                    DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                }
            }
        }
        {
            array2d<bgr_pixel> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c].red = static_cast<unsigned char>(r*14 + c + 1);
                    img[r][c].green = static_cast<unsigned char>(r*14 + c + 2);
                    img[r][c].blue = static_cast<unsigned char>(r*14 + c + 3);
                }
            }

            save_png(img, "test.png");

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_png(img, "test.png");

            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c].red == r*14 + c + 1);
                    DLIB_TEST(img[r][c].green == r*14 + c + 2);
                    DLIB_TEST(img[r][c].blue == r*14 + c + 3);
                }
            }
        }
#endif // DLIB_PNG_SUPPORT



        {
            array2d<unsigned short> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c] = static_cast<unsigned short>(r*14 + c + 0xF0);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            istringstream sin(sout.str());

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_dng(img, sin);
            
            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c] == r*14 + c + 0xF0);
                }
            }
        }


#ifdef DLIB_PNG_SUPPORT
        {
            array2d<unsigned short> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c] = static_cast<unsigned short>(r*14 + c + 0xF0);
                }
            }

            save_png(img, "test.png");

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_png(img, "test.png");
            
            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c] == r*14 + c + 0xF0);
                }
            }
        }
#endif // DLIB_PNG_SUPPORT



        {
            array2d<unsigned char> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c] = static_cast<unsigned char>(r*14 + c);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            save_bmp(img, sout);
            save_dng(img, sout);
            save_bmp(img, sout);
            istringstream sin(sout.str());

            for (int i = 0; i < 2; ++i)
            {
                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_dng(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST(img[r][c] == r*14 + c);
                    }
                }


                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_bmp(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST(img[r][c] == r*14 + c);
                    }
                }
            }
        }


#ifdef DLIB_PNG_SUPPORT
        {
            array2d<unsigned char> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c] = static_cast<unsigned char>(r*14 + c);
                }
            }

            save_png(img, "test.png");

            img.clear();
            DLIB_TEST(img.nr() == 0);
            DLIB_TEST(img.nc() == 0);

            load_png(img, "test.png");

            DLIB_TEST(img.nr() == 14);
            DLIB_TEST(img.nc() == 15);

            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    DLIB_TEST(img[r][c] == r*14 + c);
                }
            }

        }
#endif // DLIB_PNG_SUPPORT


        {
            // in this test we will only assign pixel values that can be
            // represented with 8 bits even though we are using a wider pixel type.
            array2d<unsigned short> img;
            img.set_size(14,15);
            for (long r = 0; r < 14; ++r)
            {
                for (long c = 0; c < 15; ++c)
                {
                    img[r][c] = static_cast<unsigned char>(r*14 + c);
                }
            }

            ostringstream sout;
            save_dng(img, sout);
            save_bmp(img, sout);
            save_dng(img, sout);
            save_bmp(img, sout);
            istringstream sin(sout.str());

            for (int i = 0; i < 2; ++i)
            {
                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_dng(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST(img[r][c] == r*14 + c);
                    }
                }


                img.clear();
                DLIB_TEST(img.nr() == 0);
                DLIB_TEST(img.nc() == 0);

                load_bmp(img, sin);

                DLIB_TEST(img.nr() == 14);
                DLIB_TEST(img.nc() == 15);

                for (long r = 0; r < 14; ++r)
                {
                    for (long c = 0; c < 15; ++c)
                    {
                        DLIB_TEST(img[r][c] == r*14 + c);
                    }
                }
            }
        }

        {
            array2d<unsigned short> img1;
            array2d<unsigned char> img2;
            img1.set_size(10,10);
            assign_all_pixels(img1, 0);

            img1[5][5] = 10000;
            img1[7][7] = 10000;

            equalize_histogram(img1, img2);

            for (long r = 0; r < img1.nr(); ++r)
            {
                for (long c = 0; c < img2.nc(); ++c)
                {
                    if ((r == 5 && c == 5) ||
                        (r == 7 && c == 7))
                    {
                        DLIB_TEST(img2[r][c] == 255);
                    }
                    else
                    {
                        DLIB_TEST(img2[r][c] == 0);
                    }
                }
            }

        }

        {
            array2d<unsigned char> img;
            img.set_size(10,10);
            assign_all_pixels(img, 0);

            assign_border_pixels(img, 2,2, 4);

            DLIB_TEST(zeros_matrix<unsigned char>(6,6) == subm(array_to_matrix(img), rectangle(2,2,7,7)));
            DLIB_TEST(uniform_matrix<unsigned char>(1,10, 4) == rowm(array_to_matrix(img), 0));
            DLIB_TEST(uniform_matrix<unsigned char>(1,10, 4) == rowm(array_to_matrix(img), 1));
            DLIB_TEST(uniform_matrix<unsigned char>(1,10, 4) == rowm(array_to_matrix(img), 8));
            DLIB_TEST(uniform_matrix<unsigned char>(1,10, 4) == rowm(array_to_matrix(img), 9));

            DLIB_TEST(uniform_matrix<unsigned char>(10,1, 4) == colm(array_to_matrix(img), 0));
            DLIB_TEST(uniform_matrix<unsigned char>(10,1, 4) == colm(array_to_matrix(img), 1));
            DLIB_TEST(uniform_matrix<unsigned char>(10,1, 4) == colm(array_to_matrix(img), 8));
            DLIB_TEST(uniform_matrix<unsigned char>(10,1, 4) == colm(array_to_matrix(img), 9));


            assign_border_pixels(img, 7, 7, 5);
            DLIB_TEST(uniform_matrix<unsigned char>(10,10, 5) == array_to_matrix(img));
            assign_border_pixels(img, 37, 47, 5);
            DLIB_TEST(uniform_matrix<unsigned char>(10,10, 5) == array_to_matrix(img));
        }

        {
            array2d<unsigned char> img;
            img.set_size(11,11);
            assign_all_pixels(img, 0);

            assign_border_pixels(img, 2,2, 4);

            DLIB_TEST(zeros_matrix<unsigned char>(7,7) == subm(array_to_matrix(img), rectangle(2,2,8,8)));
            DLIB_TEST(uniform_matrix<unsigned char>(1,11, 4) == rowm(array_to_matrix(img), 0));
            DLIB_TEST(uniform_matrix<unsigned char>(1,11, 4) == rowm(array_to_matrix(img), 1));
            DLIB_TEST(uniform_matrix<unsigned char>(1,11, 4) == rowm(array_to_matrix(img), 9));
            DLIB_TEST(uniform_matrix<unsigned char>(1,11, 4) == rowm(array_to_matrix(img), 10));

            DLIB_TEST(uniform_matrix<unsigned char>(11,1, 4) == colm(array_to_matrix(img), 0));
            DLIB_TEST(uniform_matrix<unsigned char>(11,1, 4) == colm(array_to_matrix(img), 1));
            DLIB_TEST(uniform_matrix<unsigned char>(11,1, 4) == colm(array_to_matrix(img), 9));
            DLIB_TEST(uniform_matrix<unsigned char>(11,1, 4) == colm(array_to_matrix(img), 10));

            assign_border_pixels(img, 7, 7, 5);
            DLIB_TEST(uniform_matrix<unsigned char>(11,11, 5) == array_to_matrix(img));
            assign_border_pixels(img, 70, 57, 5);
            DLIB_TEST(uniform_matrix<unsigned char>(11,11, 5) == array_to_matrix(img));
        }


    }


    template <typename T, typename pixel_type>
    void test_integral_image (
    )
    {
        dlib::rand rnd;

        array2d<pixel_type> img;
        integral_image_generic<T> int_img;

        int_img.load(img);
        DLIB_TEST(int_img.nr() == 0);
        DLIB_TEST(int_img.nc() == 0);

        // make 5 random images
        for (int i = 0; i < 5; ++i)
        {
            print_spinner();
            img.set_size(rnd.get_random_16bit_number()%200+1, rnd.get_random_16bit_number()%200+1);

            for (long r = 0; r < img.nr(); ++r)
            {
                for (long c = 0; c < img.nc(); ++c)
                {
                    img[r][c] = (int)rnd.get_random_8bit_number() - 100;
                }
            }

            int_img.load(img);
            DLIB_TEST(int_img.nr() == img.nr());
            DLIB_TEST(int_img.nc() == img.nc());

            // make 200 random rectangles
            for (int j = 0; j < 500; ++j)
            {
                point p1(rnd.get_random_32bit_number()%img.nc(), rnd.get_random_32bit_number()%img.nr());
                point p2(rnd.get_random_32bit_number()%img.nc(), rnd.get_random_32bit_number()%img.nr());
                rectangle rect(p1,p2);
                DLIB_TEST(int_img.get_sum_of_area(rect) == sum(subm(matrix_cast<T>(array_to_matrix(img)), rect)));
                rect = rectangle(p1,p1);
                DLIB_TEST(int_img.get_sum_of_area(rect) == sum(subm(matrix_cast<T>(array_to_matrix(img)), rect)));
            }

        }


    }


    class image_tester : public tester
    {
    public:
        image_tester (
        ) :
            tester ("test_image",
                    "Runs tests on the image processing objects and functions.")
        {}

        void perform_test (
        )
        {
            image_test();
            test_integral_image<long, unsigned char>();
            test_integral_image<double, int>();
            test_integral_image<long, unsigned char>();
            test_integral_image<double, float>();
        }
    } a;

}



