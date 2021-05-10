#!/usr/bin/perl

# scatterfile.pl
#
# Distributes the file $FILE among nodes specified in @donthave

# file to distribute
$file = "ray.txt";

# push list of nodes to @donthave
my @donthave;
for($i=1;$i<=29;$i++){
	push @donthave, "n$i";
}

# assume localhost has the file
my @have;
push @have,"localhost";

# do until everyone has the file
while(scalar(@donthave)>0){
	my @tmp = ();
	my @commands = ();
	my @childarray = ();
	foreach(@have){
		if(scalar(@donthave)==0){ last; }
		$me = pop(@donthave);
		#	use scp to transfer the file to the tmp directory
		push @commands, "ssh $_ \"scp /tmp/$file $me:/tmp/$file\"\n";
		push @tmp, $me;
	}
	foreach(@tmp){ push @have, $_; }

	foreach(@commands){
		# fork process so many can run at once
		my $child = fork();
		if($child==0){
			print $_;
			exec($_);
			exit 0;
		}
		else{
			push @childarray, $child;
		}
	}

	# wait for particular level to finish
	foreach(@childarray){
		my $child = waitpid($_,WNOHANG);
	}

	# seperate the levels, in case we get lost
	print "--\n";
}
