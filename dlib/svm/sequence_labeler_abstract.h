// Copyright (C) 2011  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_SEQUENCE_LAbELER_ABSTRACT_H___
#ifdef DLIB_SEQUENCE_LAbELER_ABSTRACT_H___

#include "../matrix.h"
#include <vector>
#include "../optimization/find_max_factor_graph_viterbi_abstract.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class example_feature_extractor
    {
        /*!
            WHAT THIS OBJECT REPRESENTS
                This object defines the interface a feature extractor must implement
                if it is to be used with the sequence_labeler defined at the bottom
                of this file.  
                
                The model used by sequence_labeler objects is the following.  
                Given an input sequence x, predict an output label sequence y
                such that:
                    y == argmax_y dot(w, PSI(x,y))
                    Where w is a parameter vector.

                Therefore, a feature extractor defines how the PSI(x,y) feature vector 
                is calculated.  It also defines how many output labels there are as 
                well as the order of the model.  

                Finally, note that PSI(x,y) is a sum of feature vectors, each extracted 
                at one of the positions of the input sequence x.  Each of these constituent
                feature vectors is defined by the get_features() method of this class.
        !*/

    public:
        // This should be the type of elements in the input sequence
        typedef the_type_of_elements_of_x sample_type;

        example_feature_extractor (
        ); 
        /*!
            ensures
                - this object is properly initialized
        !*/

        unsigned long num_features (
        ) const;
        /*!
            ensures
                - returns the dimensionality of the PSI() feature vector.  
        !*/

        unsigned long order(
        ) const; 
        /*!
            ensures
                - This object represents a Markov model on the output labels.
                  This parameter defines the order of the model.  That is, this 
                  value controls how many previous label values get to be taken 
                  into consideration when performing feature extraction for a
                  particular element of the input sequence.  Note that the runtime
                  of the algorithm is exponential in the order.  So don't make order
                  very large.
        !*/

        unsigned long num_labels(
        ) const; 
        /*!
            ensures
                - returns the number of possible output labels.
        !*/

        template <typename EXP>
        bool reject_labeling (
            const std::vector<sample_type>& x,
            const matrix_exp<EXP>& y,
            unsigned long position
        ) const;
        /*!
            requires
                - EXP::type == unsigned long
                  (i.e. y contains unsigned longs)
                - position < x.size()
                - y.size() == min(position, order) + 1
                - is_vector(y) == true
                - max(y) < num_labels() 
            ensures
                - for all valid i:
                    - interprets y(i) as the label corresponding to x[position-i]
                - if (the labeling in y for x[position] is always the wrong labeling) then
                    - returns true
                      (note that reject_labeling() is just an optional tool to allow you 
                      to overrule the normal labeling algorithm.  You don't have to use
                      it.  So if you don't include a reject_labeling() method in your
                      feature extractor it is the same as including one that always
                      returns false.)
                - else
                    - returns false
        !*/

        template <typename feature_setter, typename EXP>
        void get_features (
            feature_setter& set_feature,
            const std::vector<sample_type>& x,
            const matrix_exp<EXP>& y,
            unsigned long position
        ) const;
        /*!
            requires
                - EXP::type == unsigned long
                  (i.e. y contains unsigned longs)
                - position < x.size()
                - y.size() == min(position, order) + 1
                - is_vector(y) == true
                - max(y) < num_labels() 
                - set_feature is a function object which allows expressions of the form:
                    - set_features((unsigned long)feature_index, (double)feature_value);
                    - set_features((unsigned long)feature_index);
            ensures
                - for all valid i:
                    - interprets y(i) as the label corresponding to x[position-i]
                - This function computes the part of PSI() corresponding to the x[position]
                  element of the input sequence.  The features are returned as a sparse
                  vector by invoking set_feature().  For example, to set the feature with
                  an index of 55 to the value of 1 this method would call:
                    set_feature(55);
                  Or equivalently:
                    set_feature(55,1);
                  Therefore, the first argument to set_feature is the index of the feature 
                  to be set while the second argument is the value the feature should take.
                - This function only calls set_feature() once for each feature index.
                - This function only calls set_feature() with feature_index values < num_features()
        !*/

    };

// ----------------------------------------------------------------------------------------

    void serialize(
        const example_feature_extractor& item,
        std::ostream& out
    );
    /*!
        provides serialization support 
    !*/

    void deserialize(
        example_feature_extractor& item, 
        std::istream& in
    );
    /*!
        provides deserialization support 
    !*/

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

    template <
        typename feature_extractor
        >
    class sequence_labeler
    {
        /*!
            REQUIREMENTS ON feature_extractor
                It must be an object that implements an interface compatible with 
                the example_feature_extractor discussed above.

            WHAT THIS OBJECT REPRESENTS
                This object is a tool for doing sequence labeling.  In particular,
                it is capable of representing sequence labeling models such as
                those produced by Hidden Markov SVMs or Conditional Random fields.
                See the following papers for an introduction to these techniques:
                    - Hidden Markov Support Vector Machines by 
                      Y. Altun, I. Tsochantaridis, T. Hofmann
                    - Shallow Parsing with Conditional Random Fields by 
                      Fei Sha and Fernando Pereira


                The model used by this object is the following.  Given
                an input sequence x, predict an output label sequence y
                such that:
                    y == argmax_y dot(get_weights(), PSI(x,y))
                    Where PSI() is defined by the feature_extractor template
                    argument.  
        !*/

    public:
        typedef typename feature_extractor::sample_type sample_type;
        typedef std::vector<sample_type> sample_sequence_type;
        typedef std::vector<unsigned long> labeled_sequence_type;

        sequence_labeler(
        );
        /*!
            ensures
                - #get_weights().size() == fe.num_features()
                - #get_weights() == 0
                - #get_feature_extractor() will have an initial value for its type
        !*/

        sequence_labeler(
            const feature_extractor& fe,
            const matrix<double,0,1>& weights
        ); 
        /*!
            requires
                - fe.num_features() == weights.size()
            ensures
                - #get_feature_extractor() == fe
                - #get_weights() == weights
        !*/

        const feature_extractor& get_feature_extractor (
        ) const; 
        /*!
            ensures
                - returns the feature extractor used by this object
        !*/

        const matrix<double,0,1>& get_weights (
        ) const;
        /*!
            ensures
                - returns the parameter vector associated with this sequence labeler. 
                  The length of the vector is get_feature_extractor().num_features().  
        !*/

        unsigned long num_labels (
        ) const;
        /*!
            ensures
                - returns get_feature_extractor().num_labels()
                  (i.e. returns the number of possible output labels for each 
                  element of a sequence)
        !*/

        labeled_sequence_type operator() (
            const sample_sequence_type& x
        ) const;
        /*!
            requires
                - num_labels() > 0
            ensures
                - returns a vector Y of label values such that:
                    - Y.size() == x.size()
                    - for all valid i: 
                        - Y[i] == the predicted label for x[i]
                        - 0 <= Y[i] < num_labels()
        !*/

        void label_sequence (
            const sample_sequence_type& x,
            labeled_sequence_type& y
        ) const;
        /*!
            requires
                - num_labels() > 0
            ensures
                - #y == (*this)(x)
                  (i.e. This is just another interface to the operator() routine
                  above.  This one avoids returning the results by value and therefore
                  might be a little faster in some cases)
        !*/

    };

// ----------------------------------------------------------------------------------------

    template <
        typename feature_extractor
        >
    void serialize (
        const sequence_labeler<feature_extractor>& item,
        std::ostream& out
    );
    /*!
        provides serialization support 
    !*/

// ----------------------------------------------------------------------------------------

    template <
        typename feature_extractor
        >
    void deserialize (
        sequence_labeler<feature_extractor>& item,
        std::istream& in 
    );
    /*!
        provides deserialization support 
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_SEQUENCE_LAbELER_ABSTRACT_H___


