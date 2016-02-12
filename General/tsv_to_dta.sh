#!/usr/bin/env bash

if [ $# -ne 3 ]
then
    echo "Usage: tsv_to_dta.sh _stem_ _target_ _unused_features_file_"
    exit
fi

stem=$1
features=$stem.feature_names.txt
unused=copy.$3

head -1 $stem.tsv | tr '\t' '\n' > $features

rnd_tv_dta $stem.tsv $stem 1 no dta header all no no 1 0.0 1.0

attrbool $features $stem.dta $stem.attr $2

cp $3 $unused
perl -pi -e 's/\n/ never\n/g' $unused
cat $unused >> $stem.attr

rm $features
rm $unused

