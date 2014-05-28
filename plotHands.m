%%display data from leap motion

clear all
%handle storage to remove old vector plots (30 is arbitrary)
h = zeros(1,30); 

%hold figure; plot origin; fix axes
hold on
plot3(0,0,0);
grid on;
axis([-500 500 -500 500 0 1000]);
xlabel('x');
ylabel('y');
zlabel('z');

%connect to leap
leap2matlab('connect')
%pause required for the frame to not be invalid
pause(1);

R = [0 1 0;0 0 1;1 0 0];
R = R';

while(1)
    %get frame from leap
    f = leap2matlab('getframe');
       
    %remove old vectors from plot
    for i = 1:30
        if (h(i)~=0)
            delete(h(i));
            h(i) = 0;
        end
    end
    
    %loop through pointables and graph new vectors
    if (isstruct(f))
        n = length(f);
        for i = 1:n
            pos = f(i).position;
            dir = f(i).direction;
            %rotate axes such that they are in the conventional format
            pos = (R*(pos'))';
            dir = (100.*R*(dir'))';
            h(i) = quiver3(pos(1,1),pos(1,2),pos(1,3),dir(1,1),dir(1,2),dir(1,3));
        end
    end
    
    %pause momentarily
    pause(0.01);
end