#!/usr/bin/env python


### README ###
#Code to calculate the efficiency of a skim obtained with the UserCode/Leptoquarks/LeptonJetFilter/ code
#Usage ./getSkimEff.py -i inputListDir.txt -m skim_histo
# -i : the list must be in the simple format:
# /store/user/ferencek/QCD_EMEnriched_Pt20to30/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/
# /store/user/ferencek/QCD_EMEnriched_Pt30to80/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/
# /store/user/ferencek/QCD_EMEnriched_Pt80to170/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/
# /store/user/ferencek/QCD_BCtoE_Pt20to30/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/
# [..]
#each directory will contain:
#rootfile in EDM format (AOD, RECO, ..)
#[...]
#QCD_EMEnriched_Pt20to30__Summer09-MC_31X_V3_SD_Photon20-v1__GEN-SIM-RECO_49.root  
#QCD_EMEnriched_Pt20to30__Summer09-MC_31X_V3_SD_Photon20-v1__GEN-SIM-RECO_4.root  
#QCD_EMEnriched_Pt20to30__Summer09-MC_31X_V3_SD_Photon20-v1__GEN-SIM-RECO_50.roo
#[...]
#rootfiles with hitograms to calculate the skim efficiency
#[...]
#skim_histo_49.root
#skim_histo_4.root
#skim_histo_50.root
#[...]
#The option "-m" defines a string that should match with the name of the files containing the histograms
#The output will be a text file called skim_efficiency.txt in the format:
#
#/store/user/ferencek/QCD_EMEnriched_Pt20to30/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/ 0.0163
#/store/user/ferencek/QCD_EMEnriched_Pt30to80/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/ 0.0924
#/store/user/ferencek/QCD_EMEnriched_Pt80to170/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/ 0.2446
#/store/user/ferencek/QCD_BCtoE_Pt20to30/LQSkim_091005_032528/2ee22982599f527f855b1fb7db3bc7c3/ 0.0336
#where the first number is the directory of the skim for a given the dataset
#and the second number is the actual skim efficiency
#
#NOTE: If you run many filters in cascade or you change the name of the module (by default is LJFilter)
#the .py script should be changed accordingly
#


#
#---Import
import sys
import string
from optparse import OptionParser
import os.path
import os
from ROOT import *
import re
from array import array    # need arrays to pass to ROOT
import subprocess

#---Option Parser
#--- TODO: WHY PARSER DOES NOT WORK IN CMSSW ENVIRONMENT? ---#
usage = "usage: %prog [options] \nExample: \n./getSkimEff.py -i inputListDir.txt -m skim_histo"

parser = OptionParser(usage=usage)

parser.add_option("-i", "--inputListDir", dest="inputListDir",
                  help="list of all directories containing the skim efficiency histograms (full path required)",
                  metavar="LIST")

parser.add_option("-m", "--matchString", dest="matchString",
                  help="provide the string to match filenames containing skim eff histograms",
                  metavar="MATCHSTRING")

(options, args) = parser.parse_args()

if len(sys.argv)<4:
        print usage
	sys.exit()

#---Check if inputListDir file exist
if(os.path.isfile(options.inputListDir) == False):
        print "ERROR: file " + options.inputListDir + " not found"
	print "exiting..."
	sys.exit()

#---Open output file with efficiencies
outputFile = open("skim_efficiencies.txt",'w')

#---Loop over datasets
print "\n"
for n, dir in enumerate( open( options.inputListDir ) ):

        dir = string.strip(dir,"\n")
	#print dir

	if not os.path.isdir(os.path.join(dir)):
		print "WARNING: directory " + dir + " not found"
		continue
	#else:
	#	print "OK: directory" + lin +" found"

	#skimrootfiles=[]
	#for rootfile in os.listdir(dir):
	#	#print rootfile
	#	if not os.path.isfile(os.path.join(dir,rootfile)):
	#		continue
	#
	#	if options.matchString in rootfile:
	#		#print rootfile			
	#		skimrootfiles.append(os.path.join(dir,rootfile))
	#print skimrootfiles

	#hadd for histograms in each sample
	full_matchString = dir + "*" + options.matchString + "*"
	#print full_matchString
	datasetName = string.split(dir, "/" )[4]

	outputRootFileName = "skim_histo" + "___" + datasetName + ".root"
	#print outputRootFileName
	
	subprocess.call('hadd -f ' + outputRootFileName + " "  + full_matchString, shell=True)


	### Calculate efficiency from rootfiles ##
	tfile = TFile(outputRootFileName)
	nObjects = int ( tfile.GetListOfKeys().GetEntries() ) 
	#print nObjects

	if(nObjects==1):
		histo = TH1F()
		histo = tfile.Get("LJFilter/EventCount/EventCounter")
		#print histo.GetEntries()
		Nall = histo.GetBinContent(1)
		Nskim = histo.GetBinContent(2)
		efficiency = Nskim / Nall
		print "SKIM EFFICIENCY:"
		print dir,("%.04f" % efficiency)
		print >> outputFile, dir, ("%.04f" % efficiency)

	if(nObjects==2):
		histo1 = TH1F()
		histo1 = tfile.Get("LJFilter/EventCount/EventCounter")
		histo2 = TH1F()
		histo2 = tfile.Get("LJFilterPAT/EventCount/EventCounter")
		Nall = histo1.GetBinContent(1)
		Nskim = histo2.GetBinContent(2)
		efficiency = Nskim / Nall
		print "SKIM EFFICIENCY:"
		print dir,("%.04f" % efficiency)
		print >> outputFile, dir, ("%.04f" % efficiency)

	if(nObjects>2):
		print "ERROR: more than 2 histograms in " + outputRootFileName
		print "exiting..."
		sys.exit()


print ""
print "--> skim efficiencies at file: skim_efficiencies.txt ... "
print "exiting..."
