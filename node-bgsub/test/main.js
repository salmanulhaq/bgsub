var bgsub = require('/usr/lib/bgsub');

bgsub.subtract('/home/userx/Downloads/markable/image2.jpg', function(error, latency) {
	if(error){
		console.log(error);
	}
	else console.log('bg subtracted with latency: '+latency);
});