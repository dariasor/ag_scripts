#!/usr/bin/env bash

if [ $# -lt 3 ]
then
    echo "Usage: tsv_to_dta.sh _stem_ _target_ _unused_features_file_ [weight]"
    exit
fi

stem=$1
features=$stem.feature_names.txt
unused=$3.copy

head -1 $stem.tsv | tr '\t' '\n' > $features

tail -n+2 $stem.tsv > $stem.dta

attrbool $features $stem.dta $stem.attr $2 $4

cp $3 $unused
perl -pi -e 's/(.)$/$1 never/g' $unused
cat $unused >> $stem.attr

rm $features
rm $unused

