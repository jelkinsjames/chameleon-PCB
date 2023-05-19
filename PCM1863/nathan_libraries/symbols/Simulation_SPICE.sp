.subckt kicad_builtin_vdiff in+ in- out
  Emeas out 0 in+ in- 1
.ends

.subckt kicad_builtin_varistor A B threshold=1k rdyn=1 rleak=100MEG
  B1 A B I=( abs( V( A, B ) ) > {threshold} ?  ( abs( V( A, B ) ) - {threshold} ) / {rdyn} * sgn( V( A, B ) ) : 0 )
  R1 A B {rleak} //Leakage helps with convergence in case of circuit with current sources
.ends