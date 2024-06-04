#!/usr/bin/perl

use strict;
use FindBin;

my %dargs = ('-group_mode'=>'random', '-num_groups' => '30', '-last_char_idx' => '44');
my %args = %dargs;
my $parg;
for my $arg (@ARGV) {
  if (!($arg =~ /^-/)) {
    if ($parg && $parg =~ /^-/) {
      $args{$parg} = $arg;
    }
  } else {
    $args{$arg} = 1;
  }
  $parg = $arg;
}

if ($args{'-help'}) {
  print "$FindBin::Script has default args\n";
  for my $key (keys %dargs) {
    print "  $key $dargs{$key}\n";
  }
  exit 1;
}

my $lidx = $args{'-last_char_idx'};
my $groups = $args{'-num_groups'};
my $random_group_size = $args{'-group_mode'} eq 'random';
my @chars = (
   'a',
   'b',
   'c',
   'd',
   'e',
   'f',
   'g',
   'h',
   'i',
   'j',
   'k',
   'l',
   'm',
   'n',
   'o',
   'p',
   'q',
   'r',
   's',
   't',
   'u',
   'v',
   'w',
   'x',
   'y',
   'z',
   '0',
   '1',
   '2',
   '3',
   '4',
   '5',
   '6',
   '7',
   '8',
   '9',
   '+',
   '=',
   '-',
   '.',
   ',',
   '?',
   '/',
   ':',
   'ü', 
   'ö',
   'ä',
   '\'',
   '"',
   '&',
   '@',
   '(',
   ')',
   '!'
);

#for (my $i = 0; $i < $lidx && $i < scalar(@chars); ++$i) {
#  print $chars[$i] . "rand " . irandom(10) . "\n";
#}

for (my $j = 0; $j < $groups; ++$j) {
  my $gs = 5;
  if ($random_group_size) {
    $gs = irandom(10) + 1;
  } else {
    my $pgs = $args{'-group_mode'};
    if ($pgs && $pgs > 0 && $pgs < 20) {
      $gs = $pgs;
    }
  }
  for (my $k = 0; $k < $gs; ++$k) {
    print $chars[irandom($lidx)]; 
  }
  print " \n";
}

exit 0;

sub irandom {
  my ($limit) = @_;
  return int rand($limit);
}
