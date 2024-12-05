

char instMem[4096];

while (infile) {
	infile>>line;
	stringstream line2(line);
	char x;

	line2>>x;
	if (x >= '0' && x <= '9') {
		x = x - '0';
	} else if (x >= 'a' && x <= 'f') {
		x = x - 'a' + 10;
	} else {
		cout << "error interpreting instruction as hex" << endl;
        return 0;
	}
    instMem[i] = x << 4;

	line2>>x;
	if (x >= '0' && x <= '9') {
		x = x - '0';
	} else if (x >= 'a' && x <= 'f') {
		x = x - 'a' + 10;
	} else {
		cout << "error interpreting instruction as hex" << endl;
        return 0;
	}
	instMem[i] += x;
	
	cout<<instMem[i]<<endl;
	i++;
}