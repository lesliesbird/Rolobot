/* Rolobot 0.99e for TinyMUSH                                   */
/* by Leslie S. Bird 1997, 2009                                 */
/* Translated from original ARexx program written for the Amiga */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
 
#define BUFFER_SIZE 128
#define LINEBUF 8000


/* global variables from config file */
char mushname[40], hostname[40], port[10], username[40], password[40], botname[40];
char owner[40], quitcommand[80], quitmessage[80], confusion[80], keepa[40];
char sayprog[40];

/* global variables for socket input and output */
char myin[LINEBUF], sayout[LINEBUF], buffer[BUFFER_SIZE];
char nextword[80];
int tel, keepalive, paged = 0, decobject = 0, filtermine = 0;

char quit = 'N', audio = 'N';
FILE *rinfile, *routfile;

int main(int argc, char *argv[])
{
int i, j, ix, countdown;
char speak[LINEBUF];
char *rolobot_cfg = "rolobot.cfg";
FILE *config_file;

if (argc > 2)
{
                 printf("Usage: %s <configfile.cfg>\n", argv[0]);
                 exit(1); 
                 }
if (argc == 2)
{
         rolobot_cfg = argv[1];
         }
else
{
         printf ("Using default configuration file %s\n", rolobot_cfg);
         }
         

/* INITIALIZATION ROUTINES */

/* read config file, exit on error */
config_file = fopen (rolobot_cfg, "r");
if (config_file == NULL)
{
        printf ("Missing %s file! Exiting.\n", rolobot_cfg);
        return 0;
        }
fgets (&mushname[0], 40, config_file);
fgets (&hostname[0], 40, config_file);
fgets (&port[0], 10, config_file);
fgets (&username[0], 40, config_file);
fgets (&password[0], 40, config_file);
fgets (&botname[0], 40, config_file);
fgets (&owner[0], 40, config_file);
fgets (&quitcommand[0], 80, config_file);
fgets (&quitmessage[0], 80, config_file);
fgets (&confusion[0], 80, config_file);
fgets (&keepa[0], 40, config_file);
fgets (&sayprog[0], 40, config_file);
fclose (config_file);
keepalive = atoi(keepa);

/* get rid of the newline characters fgets stores in the string :( */
mushname[strlen(mushname) - 1] = '\0';
hostname[strlen(hostname) - 1] = '\0';
port[strlen(port) - 1] = '\0';
username[strlen(username) - 1] = '\0';
password[strlen(password) - 1] = '\0';
botname[strlen(botname) - 1] = '\0';
owner[strlen(owner) - 1] = '\0';
quitcommand[strlen(quitcommand) - 1] = '\0';
quitmessage[strlen(quitmessage) - 1] = '\0';
confusion[strlen(confusion) - 1] = '\0';
sayprog[strlen(sayprog) - 1] = '\0';

/* initialize socket, exit on error */
/* attempt to open connection to hostname, port and log in to bot account */
tel = socket_connect (&hostname[0], atoi(port));
printf ("Attempting to connect to %s...", mushname);
sprintf (sayout, "conn %s %s", username, password);
botput(sayout);
sprintf (sayout, "OUTPUTPREFIX <<");
botput(sayout);
sprintf (sayout, "OUTPUTSUFFIX >>");
botput(sayout);

printf ("Connection established!\n");
countdown = time(NULL) + keepalive;
/* INITIALIZATION COMPLETE */


/* do until Bot receives a QUIT instruction or connection is terminated */
while ((quit == 'N') && (recv(tel, buffer, BUFFER_SIZE - 1, 0) != 0))
{
      j = 0;
      /* grab a bufferful of text, then go through it line by line. */
      while (j <= strlen(buffer))
      {
		memset(myin, 0, LINEBUF);
		i = 1;
		do
		{
			myin[i - 1] = buffer[(i - 1) + j];
			i++;
			}
		while ((myin[i - 2] != '\n') && (i - 1 <= LINEBUF));
		j = j + (i - 1);

            /* We've got a line of input, now process it. */

            /* Find out if we've got the bot's own output and if so, ignore it */
    
            if ((match(myin, "<<\15", 0) > 0) || (match(myin, "<<\12", 0) > 0))
            {
		filtermine = 1;
		memset(myin, 0, LINEBUF);
                }             
            if ((match(myin, ">>\15", 0) > 0) || (match(myin, ">>/12", 0) > 0))
            {
		filtermine = 0;
		if (decobject > 0)
		{
			decobject = 0;
			fclose(routfile);
			}
		memset(myin, 0, LINEBUF);
                }

            /* Find out if we're @decompiling an object */
            if (decobject > 0)
            {
                        fputs (&myin[0], routfile);
                        }

            if (filtermine)
		memset(myin, 0, LINEBUF);

            /* see if someone is speaking or paging */
	
	        if (audio == 'Y')
            	{
		    strcpy (speak, myin);
		    for (ix = 1; ix <= strlen(speak); ++ix)
		    {
			if (speak[ix - 1] == '\42')
				speak[ix - 1] = ' ';
			if ((speak[ix - 1] == '\15') || (speak[ix - 1] == '\12'))
				speak[ix - 1] = '\0';
			}
		if (strlen(speak) > 0)
		{
		    	sprintf (sayout, "%s \42%s\42", sayprog, speak);
			system (sayout);
			}
		}
            
            if ((match(myin, "pages", 0)) || (match(myin, "says", 0)))
		{
	        	processtext (myin);
			countdown = time(NULL) + keepalive;
			}

	        /* put keepalive routine here, have Rolobot perform an action
            every keepalive seconds to keep from getting logged out. */
	if (time(NULL) > countdown)
	{
		botput ("@pemit me=Wake up!");
		countdown = time(NULL) + keepalive;
		}


	        if (myin != NULL)
               printf ("%s", myin);
            } 
/* end of buffer */ 
memset(buffer, 0, BUFFER_SIZE);
j = 1;
/* here's the end of the big while loop */
}


/* SHUTDOWN ROUTINES GO HERE */
shutdown (tel, SHUT_RDWR);
close (tel);
printf ("Ending session!\n");
return 0;
}
/* ------------------------------Functions------------------------------*/

/* Examine things said or paged */

processtext ()
{
/* get the speaker and also whether we were paged or not */

char cin, speaker[40], sot[LINEBUF], linein[LINEBUF], lineout[LINEBUF + 40];

int ix = 0, i = 0, ln = 0;

while (myin[ix] != ' ')
{
      speaker[ix] = myin[ix];
      ix++;
      }

speaker[ix] = '\0';

if (match(myin, "pages", 0) > 0)      
                paged = 1;
else
                paged = 0;

                
if (match(myin, quitcommand, 0) > 0)
{
               sprintf (sot, "\42%s", quitmessage);
               botput (sot);
               botput ("QUIT");
               quit = 'Y';
               return 0;
               }
if (match(myin, botname, 1) > 0)
{
/* Someone speaking TO or ABOUT the Bot! */
if (match(myin, "help", 1) > 0)
{
                sendoutput (speaker, "-------------------------------");
                if (match(speaker, owner, 0) > 0)
                {
                                   sprintf (sot, "%s view file <filename> \45[@emit]\45[@pemit]", botname);
                                   sendoutput (speaker, sot);
                                   sprintf (sot, "%s read text <filename> \45[line]", botname);
                                   sendoutput (speaker, sot);
				   sprintf (sot, "%s get ascii <mushobject>", botname);
				   sendoutput (speaker, sot);
                                   sprintf (sot, "%s @dec <mushobject>", botname);
                                   sendoutput (speaker, sot);
                                   sprintf (sot, "%s show dir", botname);
                                   sendoutput (speaker, sot);
                                   sprintf (sot, "%s exec file <filename>", botname);
                                   sendoutput (speaker, sot);
                                   sprintf (sot, "%s audio <on/off>", botname);
                                   sendoutput (speaker, sot);
                                   }              
                sprintf (sot, "%s come", botname);
                sendoutput (speaker, sot);
                sprintf (sot, "%s bug <player>", botname);
                sendoutput (speaker, sot);
                sprintf (sot, "%s summon", botname);
                sendoutput (speaker, sot);
                sendoutput (speaker, "-------------------------------");
                sendoutput (speaker, "Text enclosed in \45[brackets] are optional parameters.");
                sendoutput (speaker, "Commands are not case sensitive and can occur anywhere");
                sendoutput (speaker, "in a sentence.");
                return 0;
                }

if (match(myin, "view file", 1) > 0)
{
		memset (nextword, 0, 80);
      		getnextword ("view file");
		if (strlen(nextword) > 49)
		{
			sendoutput (speaker, "Illegal file name.");
			return 0;
			}
		rinfile = fopen (&nextword[0], "r");
		if (rinfile == NULL)
		{
			sendoutput (speaker, "I couldn't find it!");
			return 0;
			}
		cin = '\n';
		botput ("\n");
                while (cin != EOF)
                {
			if ((cin == '\12') || (cin == '\15'))
			{
				send (tel, &cin, 1, 0);
				cin = '\0';
				if (match(myin, "@emit", 1) > 0)
                        	{
                                	send (tel, "\n@emit ", 7, 0);
                                	}
                        	if (match(myin, "@pemit", 1) > 0)
                        	{
                                	sprintf (sot, "\n@pemit *%s=", speaker);
                                	send (tel, sot, strlen(sot), 0);
                                	}
                        	if ((match(myin, "@emit", 1) == 0) && (match(myin, "@pemit", 1) == 0))
                        	{
                                	prepend (speaker);
                                	}
				}
			cin = getc (rinfile);
			if ((cin == '\45') || (cin == '\133'))
				send (tel, "\45", 1, 0);
			if (cin == '\134')
				send (tel, "\134", 1, 0);
                        if (cin == ' ')
				send (tel, "\45b", 2, 0);
			else
				send (tel, &cin, 1, 0);
			}
		botput ("\n");		
		fclose (rinfile);
		return 0;
		}

if (match(myin, "show dir", 1) > 0)
{
		system ("ls >.rolo_temp");
		rinfile = fopen (".rolo_temp", "r");
                if (rinfile == NULL)
                {
                        sendoutput (speaker, "Error during directory read.");
                        return 0;
                        }
		while ((fgets (&linein[0], LINEBUF, rinfile)) != NULL)
                {
			printf ("%s", linein);
			sprintf (sot, "@pemit *%s=%s", speaker, linein);
			botput (sot);
			}
		fclose (rinfile);
		system ("rm .rolo_temp");
                return 0;
                }

if (match(myin, "@dec", 1) > 0)
{
                getnextword ("@dec");
		rinfile = fopen (&nextword[0], "r");
		if (rinfile != NULL)
		{
			sendoutput (speaker, "File exists, overwriting forbidden.");
			fclose (rinfile);
			return 0;
			}
		routfile = fopen (&nextword[0], "w");
		if (routfile == NULL)
		{
			sendoutput (speaker, "Unable to write file.");
			return 0;
			}
		sprintf (sot, "@dec %s", nextword);
		botput (sot);
		decobject = 1;
		sprintf (sot, "Finished @decompile of %s.", nextword);
		sendoutput (speaker, sot);
		return 0;
		}

if (match(myin, "come", 1) > 0)
{
                sprintf (sot, "@tel loc(*%s)", speaker);
                botput (sot);
                sprintf (sot, "\42You wanted me, %s?", speaker);
                botput (sot);
                return 0;
                }

if (match(myin, "read text", 1) > 0)
{
		memset(nextword, 0, 80);
                getnextword ("read text");
                if (strlen(nextword) > 49)
                {
                        sendoutput (speaker, "Illegal file name.");
                        return 0;
                        }
                rinfile = fopen (&nextword[0], "r");
                if (rinfile == NULL)
                {
                        sendoutput (speaker, "I couldn't find it!");
                        return 0;
                        }
		sprintf (sot, "@create %s", nextword);
		botput (sot);
		if (match(myin, "line", 1) > 0)
		{
			ln = 0;
			while ((fgets (&linein[0], LINEBUF, rinfile)) != NULL)
			{
                        	linein[strlen(linein) - 1] = '\0';
				ln++;
                        	sprintf (sot, "&LINE%i %s=%s", ln - 1, nextword, linein);
				botput (sot);
				}
			sprintf (sot, "&linenum %s=%i", nextword, ln);
			botput (sot);
			sprintf (sot, "@desc %s=[repeat(-,79)]", nextword);
			botput (sot);
			sprintf (sot, "@adesc %s=@dolist lnum(v(linenum))=@pemit \45#=[v(LINE[##])]", nextword);
			botput (sot);
			sprintf (sot, "@set %s=CHOWN_OK", nextword);
			botput (sot);
			sprintf (sot, "give *%s=%s", speaker, nextword);
			botput (sot);
			sendoutput (speaker, "Your text is contained on this object line by line!");
			}
		else
		{	
			sprintf (sot, "@desc %s=", nextword);
			send (tel, sot, strlen(sot), 0);
			while ((fgets (&linein[0], LINEBUF, rinfile)) != NULL)
                        {
				memset (lineout, 0, LINEBUF);
                                linein[strlen(linein) - 1] = '\0';
                                ix = 0;
                                for (i = 1; i <= strlen(linein); ++i)
                                {
                                        if ((linein[i - 1] == '\45') || (linein[i - 1] == '\133'))
                                        {
                                                lineout[ix] = lineout[ix] + '\45';
                                                ix = ix + 1;
                                                }
                                        if (linein[i - 1] == '\134')
                                        {
                                                lineout[ix] = lineout[ix] + '\134';
                                                ix = ix + 1;
                                                }
					if (linein[i - 1] == ' ')
					{
						lineout[ix] = lineout[ix] + '\45';
						lineout[ix + 1] = lineout[ix + 1] + 'b';
						ix = ix + 2;	
						}
					if (linein[i - 1] != ' ')
					{
                                        	lineout[ix] = linein[i - 1];
                                        	ix = ix + 1;
						}
                                        }
                                lineout[ix] = '\0';
				send (tel, lineout, strlen(lineout), 0);
				send (tel, "\45r", 2, 0);
				}
			send (tel, "\n", 1, 0);
			sprintf (sot, "@set %s=CHOWN_OK", nextword);
			botput (sot);
			sprintf (sot, "give *%s=%s", speaker, nextword);
			botput (sot);
			sendoutput (speaker, "Your text is contained in the @desc of this object!");

			}
		fclose (rinfile);
		return 0;
                }

if (match(myin, "exec file", 1) > 0)
{
		if (match(speaker, owner, 0) == 0)
		{
			sendoutput (speaker, "You're not the boss of me!");
			return 0;
			}
		memset(nextword, 0, 80);
                getnextword ("exec file");
                if (strlen(nextword) > 49)
                {
                        sendoutput (speaker, "Illegal file name.");
                        return 0;
                        }
                rinfile = fopen (&nextword[0], "r");
                if (rinfile == NULL)
                {
                        sendoutput (speaker, "I couldn't find it!");
                        return 0;
                        }
		cin = '\0';		
                while (cin != EOF)
                {
			cin = getc (rinfile);
			send (tel, &cin, 1, 0);
			}
		fclose (rinfile);	
                sprintf (sot, "Finished executing file %s", nextword);
                sendoutput (speaker, sot);
		return 0;
                }

if (match(myin, "get ascii", 1) > 0)
{
		memset(nextword, 0, 80);
        	getnextword ("get ascii");
                rinfile = fopen (&nextword[0], "r");
                if (rinfile != NULL)
                {
                        sendoutput (speaker, "File exists, overwriting forbidden.");
                        fclose (rinfile);
                        return 0;
                        }
                routfile = fopen (&nextword[0], "w");
                if (routfile == NULL)
                {
                        sendoutput (speaker, "Unable to write file.");
                        return 0;
                        }

        	sprintf (sot, "look %s", nextword);
		botput (sot);
		decobject = 1;
                sprintf (sot, "Saved ASCII description of %s.", nextword);
                sendoutput (speaker, sot);
		return 0;
		}

if (match(myin, "audio on", 1) > 0)
{
		if (strlen(sayprog) == 1)
		{
			sendoutput (speaker, "Sorry, speech synthesis is disabled.");
			return 0;
			}		
		audio = 'Y';
		sendoutput (speaker, "Audio mode engaged!");
		return 0;
		}

if (match(myin, "audio off", 1) > 0)
{
		audio = 'N';
		sendoutput (speaker, "Audio mode disengaged.");
		return 0;
		}

if (match(myin, "bug", 1) > 0)
{
                 getnextword ("bug");
                 sprintf (sot, "@tel loc(*%s)", nextword);
                 botput (sot);
                 sprintf (sot, "\42%s, %s sent me to you. Is there something I can help with?", nextword, speaker);
                 botput (sot);
                 return 0;
                 }

if (match(myin, "summon", 1) > 0)
{
                sendoutput (speaker, "I'll try, but I can't promise you'll be heard.");
		printf ("\7 \7 \7 \7 \7 \7 \7 \7 \7 \7 \7 \7 \7 \7 \7 \n");
                return 0;
                }

if ((match(myin, "bot", 1) > 0) || (match(myin, "robot", 1) > 0))
{
                sprintf (sot, "Yep, that's what I am, %s.", speaker);
                sendoutput (speaker, sot);
                return 0;
                }


}                 
/* Robot doesn't understand what to do */
if (paged == 1)
{
          sprintf (sot, "p %s=I'm sorry, I don't understand. Try asking me for my help commands?", speaker);
          botput (sot);
          }
else
{
          sprintf (sot, ":%s", confusion);
          botput (sot);
          }
return 0;
}
/* Easy output with newline included*/

botput (char s[])
{

char sot[LINEBUF];
sprintf (sot, "%s\n", s);
send (tel, sot, strlen(sot), 0);
memset (sot, 0, LINEBUF);
return 0;
}

/* output either paged or not with newline included */

sendoutput (char speaker[], char s[])
{

char sot[LINEBUF];
if (paged == 1)
{
          sprintf (sot, "p %s=%s\n", speaker, s);
          send (tel, sot, strlen(sot), 0);
          }
else
{
          sprintf (sot, "\42%s\n", s);
          send (tel, sot, strlen(sot), 0);
          }
memset (sot, 0, LINEBUF);
return 0;
}

/* prepare to send characters to speaker */

prepend (char speaker[])
{
char sot[LINEBUF];
if (paged == 1)
{
	sprintf (sot, "\np %s=%b", speaker);
	send (tel, sot, strlen(sot), 0);
	}
else
{
	send (tel, "\n\42", 2, 0);
	}
memset (sot, 0, LINEBUF);
return 0;
}

/* Get a word from input */

getnextword (char wordin[])
{

int i = 0, ix;
memset (nextword, 0, 80);
if (match(myin, wordin, 1) > 0)
{
                 ix = (match(myin, wordin, 1) + 2);

                 while ((myin[ix] != ' ') && (myin[ix] != '\12') && (myin[ix] != '\15') && (myin[ix] != '\47') && (myin[ix] != '\42') && (myin[ix] != '\0'))
                 {
                       nextword[i] = myin[ix];
                       ix++;
                       i++;
                       }
                 nextword[i] = '\0';
                 return 0;
                 }
}
