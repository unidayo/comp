CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"
CMAKE_BUILD_DIR="/home/wsl2ubuntu/comp/build"

cd $CONTESTS_DIR
acc new $1 --choice all
cmake $CMAKE_BUILD_DIR
cd $1
for file_path  in `(find -name "*.cpp") | sort`; do
    sleep 0.1
    code ${file_path}
done