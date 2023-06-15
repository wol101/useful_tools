../conversion_progs/convert_to_xan --swapYX --flipX --offsetZ -0.0835 < ../original/left_whole_foot.txt > LeftFoot.xan
../conversion_progs/convert_to_xan --swapYX --flipX --offsetZ -0.0835 < ../original/left_tib_fib.txt > LeftLeg.xan
../conversion_progs/convert_to_xan --swapYX --flipX --offsetZ -0.0835 < ../original/left_femur.txt .txt > LeftThigh.xan
../conversion_progs/convert_to_xan --swapYX --flipX --flipOrder --offsetZ 0.0835 < ../original/right_whole_foot.txt > RightFoot.xan
../conversion_progs/convert_to_xan --swapYX --flipX --flipOrder --offsetZ 0.0835 < ../original/right_tib_fib.txt > RightLeg.xan
../conversion_progs/convert_to_xan --swapYX --flipX --flipOrder --offsetZ 0.0835 < ../original/right_femur.txt .txt > RightThigh.xan
../conversion_progs/convert_to_xan --swapYX --flipX --flipOrder --offsetX -0.0707 --offsetY 0.0661 < ../original/whole_pelvis.txt .txt > Torso.xan
