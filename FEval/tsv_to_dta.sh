#!/usr/bin/env bash

if [ $# -ne 5 ]
then
    echo "Usage: tsv_to_dta.sh _stem_ _target_ _unused_features_file_ group|no _group_column_"
    exit
fi

stem=$1
features=$stem.feature_names.txt
unused=copy.$3

head -1 $stem.tsv | tr '\t' '\n' > $features

rnd_tv_dta $stem.tsv $stem 1 no dta header no no $4 $5 0.2 0.8

attrbool $features $stem.train.dta $stem.attr $2

cp $3 $unused
perl -pi -e 's/\n/ never\n/g' $unused
cat $unused >> $stem.attr

rm $features
rm $unused

