CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"
CMAKE_BUILD_DIR="/home/wsl2ubuntu/comp/build"
TEMPLATE_PATH="/home/wsl2ubuntu/comp/acc-config/cpp/main.cpp"

cd $CONTESTS_DIR
mkdir $1
cd $1
for alp in {a..h}; do
    cp TEMPLATE_PATH ${1}_${alp}.cpp;
done
cmake $CMAKE_BUILD_DIR
cd $1
for file_path  in `(find -name "*.cpp") | sort`; do
    sleep 0.1
    code ${file_path}
done