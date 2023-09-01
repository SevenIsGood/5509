SECTIONS
{
    .text   : > SARAM
    .switch : > SARAM
    .bss    : > DARAM
    .cinit  : > SARAM
    .pinit  : > SARAM
    .const  : > DARAM
    .data   : > DARAM
    .cio    : > SARAM

    .printf : > SARAM
    
    /* grammar data */
    /* .grammar_data   : > SARAM */
    .gd_config      : > SARAM
    .gd_dim         : > SARAM
    .gd_gconst      : > SARAM
    .gd_hmm         : > SARAM
    .gd_mixture     : > SARAM
    .gd_mu          : > DARAM //SARAM
    .gd_net         : > SARAM
    .gd_pdf         : > SARAM
    .gd_tran        : > SARAM
    .gd_var         : > SARAM
    .gd_word        : > SARAM

    .charrom > Char_ROM
}
