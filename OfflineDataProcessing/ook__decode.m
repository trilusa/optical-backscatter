load_distance_signals
d=4;
startidx = 1068; %for example
endidx = 20450; %for example
symT = 1/19.29573;
sampT = 7.1064e-04;
symN = ceil(symT/sampT);
thresh = 45000;

i = 1;
msg=[];
msg_sent = [1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, ...
         1,1,0,0,1,0,0,1,0,0,0,0,1,1,1,1,1,1,0,1,1,0,1,0,...
         1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,1,1,0,1,0,0,0,1,...
        1,0,0,0,0,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,1,0,0,1,1,...
        0,0,0,1,1,0,0,1,1,0,0,0,1,0,1,0,0,0,1,0,1,1,1,0,0,1];

t=distance_signals{d}.t_ms/1000;
y=(distance_signals{d}.v);
y=y(startidx:endidx);
y=smooth(y,21);

while i < length(msg_sent) * symN
    sym=y(i:i+symN);
    sym=sum(sym)/symN;
    if(sym > thresh)
        msg = [msg '1'];
    else
        msg = [msg '0'];
    end
    i = i+symN;
end

 msg_cell = num2cell(msg);
 msg_num=[];
 for i = 1:length(msg)
    msg_num = [msg_num str2num(msg_cell{i})];
 end
%  msg_num
error = msg_sent - msg_num;
BER=sum(abs(error))/length(msg_sent)
