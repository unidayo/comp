VCONTESTS_DIR="/home/wsl2ubuntu/comp/virtual_contests"

cd $VCONTESTS_DIR
echo "VコンテストのURL"
read vcontest_url
oj-api get-contest $vcontest_url | jq -rc '.result.name, .result.problems[]' | while IFS= read -a name data; do
    echo $name
    mkdir "$name"
    cd "$name"
    while IFS= read -r data; do
        echo $data
        url=$(echo $data | jq -r .url)
        taskname=$(echo $data |jq -r .name)
        mkdir "$taskname"
        cd "$taskname"
        oj download "$url"
        cd ..
    done 
done