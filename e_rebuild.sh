# Rebuilds database

# Call it with "source e_rebuild.sh" in the directory where the files are.

./bin/e_rebuild_data
mv data.tmp data.dbc
rm -rf data.cdx
./bin/e_rebuild_index
