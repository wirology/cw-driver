Training utility run_text.sh allows playing training texts on soundcard. 

$> ./run_text.sh <training_text_file> [speed in wpm]

Use random group generator for randomized input. Examples:

$> ./random_group_generator.pl > random_text.txt

Random generator has some mode switches. Default generates random group size 1-10 and 44 characters.

$> ./random_group_generator.pl -num_groups 3
8ljlx-r 
.xdx=d 
t 

Group mode allows setting a fixed group size.
$> ./random_group_generator.pl -num_groups 5 -group_mode 5
an:-o 
-ga0v 
,e9hs 
cjo-5 
zh?hb 

Last char index allows training a subset of characters
$> ./random_group_generator.pl -num_groups 4 -group_mode 5 -last_char_idx 10
geccj 
hahbf 
ahaia 
cicgi  

or a larger set
$> ./random_group_generator.pl -num_groups 4 -group_mode 5 -last_char_idx 50
öx'd8 
ofz7h 
&660c 
/lcy2 

