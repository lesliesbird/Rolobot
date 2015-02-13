/* RoloBot.arexx for TinyMUSH */
/* included for historical purposes only. Use at your own risk. */

OPTIONS RESULTS

QUIT = "NO"
Paged = "NO"
Audio = "NO"
say "Attemping to connect to Tiny MUSH..."
if ~open(TelSess,'TCP:tinymush.org/6250') then do
    say "Connection not responding!"
    exit
    end
CALL WRITELN(TelSess,"conn Rolo potrzebie")
call writeln(TelSess,"OUTPUTPREFIX <<")
call writeln(TelSess,"OUTPUTSUFFIX >>")
say "Connection established!"


do until QUIT = "YES"
  MyIn = readln(TelSess)
  MyIn2 = upper(MyIn)

  Call FilterMine
  if (left(MyIn,2) = ">>") | (left(MyIn,2) = "<<") then do
     MyIn = ""
  end



/* See if someone is speaking or paging */
 
  if (Audio = "YES") then do
     SayOut = compress(MyIn,d2c(34)"<>")
     address COMMAND "say "d2c(34)SayOut d2c(34)
  end

  if index(MyIn,"pages") > 0 | index(MyIn,"says") > 0 then do
     Call ProcessText
  end

  if index(MyIn,"finished Dumping.") > 0 then do
     call writeln(TelSess,"WHO")
  end
  say MyIn
end


say "Ending session!"
call close(TelSess)
exit

/* -----------------------------Subroutines----------------------*/
   
/* Eliminate my Own Output! */

FilterMine:
If left(MyIn,2) = "<<" then do
  do until left(MyIn,2) = ">>"
    say MyIn
    MyIn = readln(TelSess)
    end
end
return

/* Examine things said or paged */

ProcessText:
Speaker = word(MyIn,1)

if index(MyIn,"pages") > 0 then do
   Paged = "YES"
end
else do
   Paged = "NO"
end

If index(MyIn,"Rolo QUIT") > 0 then do
  call writeln(TelSess,d2c(34)"Oh Boo hoo! Nobody loves me!")
  call writeln(TelSess,"QUIT")
  QUIT = "YES"
  return
end


if index(MyIn2,"ROLO") > 0 then do
  Call RobotCommands
end

return

/* Someone speaking TO or ABOUT the Bot! */
RobotCommands:

if index(MyIn2," BOT") > 0 | index(MyIn2,"ROBOT") > 0 then do
   Outline = "Yep, that's what I am, "Speaker"."
   call SendOutPut

return
end

if index(MyIn2,"PLAY FILE") > 0 then do
   MyIn3 = ""
   Call PlayFile
   return
end

if index(MyIn2,"SHOW DIR") > 0 then do
   MyIn3 = ""
   Call ShowDir
   return
end

if index(MyIn2,"@DEC") > 0 then do
   MyIn3 = ""
   Call DumpObject
   return
end

if index(Myin2,"COME") > 0 then do
   call writeln(TelSess,"@tel loc(*"Speaker")")
   OutLine = "You wanted me, "Speaker"?"
   call writeln(TelSess,d2c(34)OutLine)
   return
end

if index(Myin2, "READ TEXT") > 0 then do
   MyIn3 = ""
   call ReadTextToObject
   return
end

if index(MyIn2, "GETIN FILE") > 0 then do
   MyIn3 = ""
   call ReadInCommandFile
   return
end

if index(MyIn2, "BUG") > 0 then do
   i = find(MyIn2,"BUG") + 1
   call GetFileName
   call writeln(TelSess,"@tel loc(*"FileIn")")
   OutLine = FileIn", "Speaker" sent me to you. Is there something I can help with?"
   call writeln(TelSess,d2c(34)OutLine)
   return
end

if index(MyIn2, "SUMMON") > 0 then do
   call AlertSound
   return
end

if index(MyIn2, "AUDIO ON") > 0 then do
   Audio = "YES"
   OutLine = "Audio mode engaged!"
   call SendOutPut
   return
end

if index(MyIn2, "AUDIO OFF") > 0 then do
   Audio = "NO"
   OutLine = "Audio mode disengaged."
   call SendOutPut
   return
end

if index(MyIn2, "HELP") > 0 then do
   OutLine = "-------------------------------"
   call SendOutPut
   OutLine = "Rolo play file <filename> %[@emit]%[@pemit]"
   call SendOutPut
   OutLine = "Rolo read text <filename> %[line]"
   call SendOutPut
   OutLine = "Rolo @dec <mushobject>"
   call SendOutPut
   OutLine = "Rolo show dir"
   call SendOutPut
   OutLine = "Rolo come"
   call SendOutPut
   OutLine = "Rolo bug <player>"
   call SendOutPut
   OutLine = "Rolo summon"
   call SendOutPut
   OutLine = "-------------------------------"
   call SendOutPut
   OutLine = "Text enclosed in %[brackets] are optional parameters."
   call SendOutPut
   OutLine = "Commands are not case sensitive and can occur anywhere in"
   call SendOutPut
   OutLine = "a sentence."
   call SendOutPut
   return
end


/* Robot doesn't understand what to do */

 If (Paged = "YES") then do
    call writeln(TelSess,"p "Speaker"=I'm sorry, I don't understand... Try asking me for my help commands?")
    end
  else do
    call writeln(TelSess,":rolls around in expectation.")
  end


return


/* Robot will recite a file off disk */

PlayFile:

i = find(MyIn2,"FILE") + 1
call GetFileName

if open(PFile,FileIn,'R') then do


do until eof(Pfile)

MyIn3 = readln(Pfile)
MyIn4 = ""
do i = 1 to length(MyIn3)
   j = substr(MyIn3,i,1)
   if j = "%" then j = "%%"
   if j = "[" then j = "%["
   if j = "\" then j = "\\"

MyIn4 = MyIn4||j
end
MyIn3 = MyIn4


if index(MyIn2,"@EMIT") > 0 then do
       call writeln(TelSess,"@emit "MyIn3)
    end
    else do

    if index(MyIn2,"@PEMIT") > 0 then do
       call writeln(TelSess,"@pemit *"Speaker"="MyIn3)
    end

    else do
       Outline = MyIn3
       call SendOutPut
    end
end

end
call close(Pfile)
end
else do
   Outline = "I couldn't find it!"
   call SendOutPut
end

return

DumpObject:

i = find(MyIn2,"@DEC") + 1

call GetFileName

if open(PFile,FileIn,'W') then do

   call writeln(TelSess,"@dec "FileIn)


   do until left(MyIn3,2) = ">>"
      MyIn3 = readln(TelSess)
      say MyIn3
      if MyIn3 = "<<" then MyIn3 = ""
      call writeln(PFile,MyIn3)
   end
   Outline = "Finished @decompile of "FileIn" to filename "FileIn"."
   call SendOutPut
   call close(PFile)
end
else do
   Outline = "Error in creating filename "FileIn". Please tell a wizard."
   call SendOutPut

end

return

GetFileName:

if (i < words(MyIn)) then do
    FileIn = word(MyIn,i)
    end

else do

    FileIn1 = word(MyIn,i)
    if (Paged = "YES") then do
       FileIn = left(FileIn1,length(FileIn1) - 1)
       end
    else do
       FileIn = left(FileIn1,length(FileIn1) - 2)
       end
end

return

SendOutPut:

if (Paged = "YES") then do

     call writeln(TelSess,"p "Speaker"="Outline)
     end
else do
     call writeln(TelSess,d2c(34)Outline)
end
return

ShowDir:

address COMMAND 'list >ram:rolo_temp'

if open(PFile,'ram:rolo_temp','R') then do
   do until eof(PFile)

   MyIn3 = readln(PFile)
   say MyIn3
   call writeln(TelSess,"@pemit *"Speaker"="MyIn3)
   end
   call close(PFile)
address COMMAND 'delete ram:rolo_temp'
end
else do
   Outline = "Error during directory read. Please tell a wizard!"
   call SendOutPut
end
return

ReadTextToObject:

i = find(MyIn2,"TEXT") + 1
call GetFileName

if open(PFile,FileIn,'R') then do
call writeln(TelSess,"@create "FileIn)
if index(MyIn2,"LINE") > 0 then do
  k = 0
  do until eof(PFile)
     k = k + 1
     MyIn4 = readln(PFile)
     MyIn3 = ""
     do i = 1 to length(MyIn4)
        j = substr(MyIn4,i,1)
        if j = "%" then j = "%%"
        if j = "[" then j = "%["
        if j = "\" then j = "\\"
        MyIn3 = MyIn3||j
        end
     call writeln(TelSess,"&LINE"strip(k)" "FileIn"="MyIn3)
  end
  call writeln(TelSess,"&linenum "FileIn"="strip(k))
  call writeln(TelSess,"@desc "FileIn"=[repeat(-,79)]")
  call writeln(TelSess,"@adesc "FileIn"=@dolist lnum(v(linenum))=@pemit %#=[v(LINE[##])]")
  call writeln(TelSess,"@set "FileIn"=CHOWN_OK")
  call writeln(TelSess,"give *"Speaker"="FileIn)
  OutLine = "Your text is contained on this object line by line!"
  call SendOutPut
  call close(PFile)
end
else do
   do until eof(PFile)

   MyIn4 = readch(PFile)
     if MyIn4 = "%" then MyIn4 = "%%"
     if MyIn4 = "[" then MyIn4 = "%["
     if MyIn4 = "\" then MyIn4 = "\\"
     if (MyIn4 = d2c(13)) | (MyIn4 = d2c(10)) then MyIn4 = "%r"
   MyIn3 = MyIn3||MyIn4

   end

   call writeln(TelSess,"@desc "FileIn"="MyIn3)
   call writeln(TelSess,"@set "FileIn"=CHOWN_OK")
   call writeln(TelSess,"give *"Speaker"="FileIn)
   OutLine = "Your text is contained in the @desc of this object!"
   call SendOutPut
   call close(PFile)
   end
end

else do

OutLine = "I couldn't find it!"
call SendOutPut
end

return

ReadInCommandFile:

i = find(MyIn2,"FILE") + 1
call GetFileName
if open(PFile,FileIn,'R') then do
   do until eof(PFile)

   MyIn3 = readln(PFile)
   call writeln(TelSess,MyIn3)
   end
   OutLine = "Finished reading in "FileIn"."
   call SendOutPut
   call close(PFile)
end
else do
   OutLine = "I couldn't find it!"
   call SendOutPut
end

return

SMTPMail:

if index(MyIn2,"USER") > 0 then do
   i = find(MyIn2,"USER") + 1
   call GetFileName
   UserName = FileIn
   i = find(MyIn2,"FILE") + 1
   call GetFileName
   address COMMAND "SMTPpost -f Rolo -R Rolo -s "FileIn" -t "UserName" -raw "FileIn
   OutLine = "File "FileIn" sent to "UserName"."
   call SendOutPut
end
else do
   OutLine = "I don't know who to send it to!"
   call SendOutPut
end
return

GetFTP:

if index(MyIn2,"FROM") > 0 then do
i = find(MyIn2,"FROM") + 1
call GetFileName
SiteName = FileIn
i = find(MyIn2,"FILE") + 1
call GetFileName

if open(PFile,"ram:rolo_FTP",'W') then do
   writeln(PFile,"open "SiteName)
   writeln(PFile,"cd /")
   writeln(PFile,"mget "FileIn)
   writeln(PFile,"bye")
   writeln(PFile,"")
   close(PFile)
   address COMMAND 'ncftp <ram:rolo_FTP'
   address COMMAND 'delete ram:rolo_FTP'
   OutLine = "Finished getting file "FileIn"."
   call SendOutPut
end
else do
   OutLine = "Error creating file rolo_FTP. Please tell a wizard!"
   call SendOutPut
end
end

else do
   OutLine = "I don't know where to find it!"
   call SendOutPut
end

return

AlertSound:

OutLine = "I'll try, but I can't promise you'll be heard!"
call SendOutPut
address COMMAND 'oplay hd1:http/htdocs/sounds/amigasamples/minimal.iff'
return

