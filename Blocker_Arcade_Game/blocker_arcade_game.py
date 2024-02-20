from tkinter import *
from random import choice
import time

def clock():
    return time.perf_counter()

# sx and sy are the horizontal and vertical speed of the ball
# in pixels per animation step
block, ball, sx, sy, goalSpeed = None, None, 5, 5, 1 #5 steps to the right and down
fieldSize, goalSize, ballSize = 300, 25, 10

roundCounter = 0

# returns position of the *center* of the ball
def ballPosition():
    x1, y1, x2, y2 = list(field.coords(ball))
    return [(x1+x2)/2, (y1+y2)/2]

def randomBallPosition():
    return [choice(list(range(fieldSize-ballSize)))
            for n in range(2)]

def willScore(x, y):
    x += ballSize/2
    y += ballSize/2
    return abs(x-y) > fieldSize - 2*goalSize
    
def startGame():
    global startTime, ball, block, roundCounter, totalTime
    roundCounter += 1
    if roundCounter == 1:
        totalTime = 0
    # remember to delete block and ball from previous game
    if block:
        field.delete(block)
    block = None
    if ball:
        field.delete(ball)
    # place ball at random, but it will not automatically score
    upperLeftX, upperLeftY = randomBallPosition()
    while willScore(upperLeftX, upperLeftY):
        upperLeftX, upperLeftY = randomBallPosition()        
    ball = field.create_oval(upperLeftX, upperLeftY,
                             upperLeftX+ballSize, upperLeftY+ballSize,
                             fill='blue')
    startTime = clock()
    animate()

def goalLeftX():
    return list(field.coords(goal))[0]
     
def animate():
    global sx, sy, goalSpeed, totalTime, roundCounter, startTime
    pattern = 'Round {0} - Elapsed time: {1:.1f} seconds - Total: {2:.1f} seconds'
    elapsed = clock() - startTime
    timeDisplay['text'] = pattern.format(roundCounter, elapsed, totalTime+elapsed)
    x, y = ballPosition()
    hitVertical = hitBlock() and blockType == 'vertical'
    if x+sx>fieldSize or x+sx<0 or hitVertical:
        sx *= -1
    hitHorizontal = hitBlock() and blockType == 'horizontal'
    if y+sy>fieldSize or y+sy<0 or hitHorizontal:
        sy *= -1
    if goalLeftX()+goalSpeed<0 or goalLeftX()+goalSize+goalSpeed>fieldSize:
        goalSpeed *= -1
    field.move(ball, sx, sy)
    field.move(goal, goalSpeed, 0)
    if inGoal():
        totalTime += elapsed
        if roundCounter == 5:
            pattern = 'Game Over: Total Time {0:.1f} seconds'
            timeDisplay['text'] = pattern.format(totalTime)
            roundCounter = 0
    else:
        root.after(20, animate)
        
# Only one block at a time; delete one before creating the next

def leftClick(event):
    global block, blockType
    if block:
        field.delete(block)
    block = field.create_rectangle(event.x-20, event.y,
                                   event.x+20, event.y+6,
                                   fill='light green')
    blockType = 'horizontal'
    
def rightClick(event):
    global block, blockType
    if block:
        field.delete(block)
    block = field.create_rectangle(event.x, event.y-20,
                                   event.x+6, event.y+20,
                                   fill='light green')
    blockType = 'vertical'
    
# return True if the center of the ball is inside the
# block’s boundary
def hitBlock():
    if not block:
        return False
    ballX, ballY = ballPosition()
    blockX1, blockY1, blockX2, blockY2 = field.coords(block)
    return (blockX1 <= ballX <= blockX2 and
            blockY1 <= ballY <= blockY2)

# return True if the center of the ball is inside the
# goal area
def inGoal():
    ballX, ballY = ballPosition()
    return goalLeftX() <= ballX <= goalLeftX()+goalSize and fieldSize-goalSize <= ballY <= fieldSize



root = Tk()

timeDisplay = Label(root)
timeDisplay.pack()

field = Canvas(root, width=fieldSize, height=fieldSize, bg='light blue')
field.pack()

startButton = Button(root, command=startGame, text='Go')
startButton.pack()

# the goal
goal = field.create_rectangle(0, fieldSize-goalSize, goalSize, fieldSize, fill='red')

field.bind('<ButtonPress-1>', leftClick)
field.bind('<ButtonPress-3>', rightClick)



mainloop()

