
#include "utilities.hpp"

// Test Symbols

t_symbol *ps_distance = gensym("distance");
t_symbol *ps_match = gensym("match");
t_symbol *ps_scale = gensym("scale");
t_symbol *ps_within = gensym("within");
t_symbol *ps_less = gensym("less");
t_symbol *ps_greater = gensym("greater");
t_symbol *ps_lesseq = gensym("lesseq");
t_symbol *ps_greatereq = gensym("greatereq");
t_symbol *ps_distance_ratio = gensym("distance_ratio");
t_symbol *ps_scale_ratio = gensym("scale_ratio");
t_symbol *ps_within_ratio = gensym("within_ratio");

t_symbol *ps_sym_distance = gensym("-");
t_symbol *ps_sym_match = gensym("==");
t_symbol *ps_sym_scale = gensym("%");
t_symbol *ps_sym_within = gensym("<->");
t_symbol *ps_sym_less = gensym("<");
t_symbol *ps_sym_greater = gensym(">");
t_symbol *ps_sym_lesseq = gensym("<=");
t_symbol *ps_sym_greatereq = gensym(">=");
t_symbol *ps_sym_distance_ratio = gensym("/");
t_symbol *ps_sym_scale_ratio = gensym("%%");
t_symbol *ps_sym_within_ratio = gensym("</>");

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
