
#include "utilities.h"

// Test Symbols

t_symbol *ps_distance;
t_symbol *ps_match;
t_symbol *ps_scale;
t_symbol *ps_within;
t_symbol *ps_less;
t_symbol *ps_greater;
t_symbol *ps_lesseq;
t_symbol *ps_greatereq;
t_symbol *ps_distance_ratio;
t_symbol *ps_scale_ratio;
t_symbol *ps_within_ratio;

t_symbol *ps_sym_distance;
t_symbol *ps_sym_match;
t_symbol *ps_sym_scale;
t_symbol *ps_sym_within;
t_symbol *ps_sym_less;
t_symbol *ps_sym_greater;
t_symbol *ps_sym_lesseq;
t_symbol *ps_sym_greatereq;
t_symbol *ps_sym_distance_ratio;
t_symbol *ps_sym_scale_ratio;
t_symbol *ps_sym_within_ratio;

// Init Symbols

void init_test_symbols()
{
    ps_distance = gensym("distance");
    ps_match = gensym("match");
    ps_scale = gensym("scale");
    ps_within = gensym("within");
    ps_less = gensym("less");
    ps_greater = gensym("greater");
    ps_lesseq = gensym("lesseq");
    ps_greatereq = gensym("greatereq");
    ps_distance_ratio = gensym("distance_ratio");
    ps_scale_ratio = gensym("scale_ratio");
    ps_within_ratio = gensym("within_ratio");
    
    ps_sym_distance = gensym("-");
    ps_sym_match = gensym("==");
    ps_sym_scale = gensym("%");
    ps_sym_within = gensym("<->");
    ps_sym_less = gensym("<");
    ps_sym_greater = gensym(">");
    ps_sym_lesseq = gensym("<=");
    ps_sym_greatereq = gensym(">=");
    ps_sym_distance_ratio = gensym("/");
    ps_sym_scale_ratio = gensym("%%");
    ps_sym_within_ratio = gensym("</>");
}

// Attempts to match an atom with any of the symbols representing a valid test type

TestType entrymatcher_test_types(t_atom *argv)
{
    if (atom_gettype(argv) != A_SYM)
        return TEST_NONE;
    
    if (atom_getsym(argv) == ps_match || atom_getsym(argv) == ps_sym_match) return TEST_MATCH;
    if (atom_getsym(argv) == ps_distance || atom_getsym(argv) == ps_sym_distance) return TEST_DISTANCE;
    if (atom_getsym(argv) == ps_less || atom_getsym(argv) == ps_sym_less) return TEST_LESS_THAN;
    if (atom_getsym(argv) == ps_greater || atom_getsym(argv) == ps_sym_greater) return TEST_GREATER_THAN;
    if (atom_getsym(argv) == ps_lesseq || atom_getsym(argv) == ps_sym_lesseq) return TEST_LESS_THAN_EQ;
    if (atom_getsym(argv) == ps_greatereq || atom_getsym(argv) == ps_sym_greatereq) return TEST_GREATER_THAN_EQ;
    if (atom_getsym(argv) == ps_scale || atom_getsym(argv) == ps_sym_scale) return TEST_SCALE;
    if (atom_getsym(argv) == ps_within || atom_getsym(argv) == ps_sym_within) return TEST_WITHIN;
    if (atom_getsym(argv) == ps_distance_ratio || atom_getsym(argv) == ps_sym_distance_ratio) return TEST_DISTANCE_RATIO;
    if (atom_getsym(argv) == ps_scale_ratio || atom_getsym(argv) == ps_sym_scale_ratio) return TEST_SCALE_RATIO;
    if (atom_getsym(argv) == ps_within_ratio || atom_getsym(argv) == ps_sym_within_ratio) return TEST_WITHIN_RATIO;
    
    return TEST_NONE;
}

