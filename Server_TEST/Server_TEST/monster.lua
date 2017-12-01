myid =99999;
playerID=9999;

function set_uid(x)
	myid =x;
end


function awake_monster()
	API_awake_monster(myid,1000);
end