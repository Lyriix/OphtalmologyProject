#!/bin/bash

VERSION="0.0.0"

# trap keyboard interrupt ctrl-c
trap control_c SIGINT

function setPath {
    cat <<SETPATH
    --------------------------------------------------------------------------------------
Error locating ANTS
--------------------------------------------------------------------------------------
It seems that the ANTSPATH environment variable is not set. Please add the ANTSPATH
variable. This can be achieved by editing the .bash_profile in the home directory.
Add:

ANTSPATH=/home/yourname/bin/ants/

Or the correct location of the ANTS binaries.

Alternatively, edit this script ( `basename $0` ) to set up this parameter correctly.

SETPATH
    exit 1
    
}

export ANTSPATH=${ANTSPATH="/home/charly/softwares/ANTS/ANTs_BIN/bin"} #TO EDIT

if [[ ${#ANTSPATH} -le 3 ]];
    then 
        setPath >&2
fi

ANTS=${ANTSPATH}/antsRegistration
if [[ ! -s ${ANTS} ]];
    then
        echo "antsRegistration progran cant be found. Please (re)define \$ANTSPATH in your environment. "
        exit
fi

function Usage {
    cat <<USAGE

Usage:

`basename $0` -d ImageDimension -f FixedImage -m MovingImage -o OutputFolder

Compulsory arguments:

     -d:  ImageDimension: 2 or 3 (for 2 or 3 dimensional registration of single volume)

     -f:  Fixed image or source image or reference image

     -m:  Moving imageS or target image
     
     -o : output folder

     # A FIXER EN DUR MovingToFixed.>>>>.nii.gz# -o:  OutputPrefix: A prefix that is prepended to all output files.

Optional arguments:

     -n:  Number of threads (default = 1)

     -r:  radius for cross correlation metric used during SyN stage (default = 4)

     -s:  spline distance for deformable B-spline SyN transform (default = 26)

     -x:  mask for the fixed image space

     -p:  precision type (default = 'd')
        f: float
        d: double

     -j:  use histogram matching (default = 0)
        0: false
        1: true

     NB:  Multiple image pairs can be specified for registration during the SyN stage.
          Specify additional images using the '-m' and '-f' options.  Note that image
          pair correspondence is given by the order specified on the command line.
          Only the first fixed and moving image pair is used for the linear resgitration
          stages.

Example:

`basename $0` -d 3 -f fixedImage.nii.gz -m movingImage.nii.gz 

--------------------------------------------------------------------------------------
ANTs was created by:
--------------------------------------------------------------------------------------
Brian B. Avants, Nick Tustison and Gang Song
Penn Image Computing And Science Laboratory
University of Pennsylvania

USAGE
    exit 1
}

function Help {
    cat <<HELP

Usage:

`basename $0` -d ImageDimension -f FixedImage -m MovingImage -

Example Case:

`basename $0` -d 3 -f fixedImage.nii.gz -m movingImage.nii.gz 

Compulsory arguments:

     -d:  ImageDimension: 2 or 3 (for 2 or 3 dimensional registration of single volume)

     -f:  Fixed image or source image or reference image

     -m:  Moving imageS or target imageS

     -o : output folder

Optional arguments:

     -n:  Number of threads (default = 1)

     -r:  radius for cross correlation metric used during SyN stage (default = 4)

     -s:  spline distance for deformable B-spline SyN transform (default = 26)

     -x:  mask for the fixed image space

     -p:  precision type (default = 'd')
        f: float
        d: double

     -j:  use histogram matching (default = 0)
        0: false
        1: true
     

     NB:  Multiple image pairs can be specified for registration during the SyN stage.
          Specify additional images using the '-m' and '-f' options.  Note that image
          pair correspondence is given by the order specified on the command line.
          Only the first fixed and moving image pair is used for the linear resgitration
          stages.

--------------------------------------------------------------------------------------
Get the latest ANTs version at:
--------------------------------------------------------------------------------------
https://github.com/stnava/ANTs/

--------------------------------------------------------------------------------------
Read the ANTS documentation at:
--------------------------------------------------------------------------------------
http://stnava.github.io/ANTs/

--------------------------------------------------------------------------------------
ANTS was created by:
--------------------------------------------------------------------------------------
Brian B. Avants, Nick Tustison and Gang Song
Penn Image Computing And Science Laboratory
University of Pennsylvania

Relevent references for this script include:
   * http://www.ncbi.nlm.nih.gov/pubmed/20851191
   * http://www.frontiersin.org/Journal/10.3389/fninf.2013.00039/abstract
--------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------

HELP
    exit 1
}

function reportMappingParameters {
    cat <<REPORTMAPPINGPARAMETERS

--------------------------------------------------------------------------------------
 Mapping parameters
--------------------------------------------------------------------------------------
 ANTSPATH is $ANTSPATH

 Dimensionality:           $DIM
 Output folder             $OUTPUTFOLDER
 Fixed images:             ${FIXEDIMAGE[@]}
 Moving images:            ${MOVINGIMAGES[@]}
 Number of threads:        $NUMBEROFTHREADS
 Spline distance:          $SPLINEDISTANCE
 Transform type:           $TRANSFORMTYPE
 CC radius:                $CCRADIUS
 Precision:                $PRECISIONTYPE
 Use histogram matching    $USEHISTOGRAMMATCHING
======================================================================================
REPORTMAPPINGPARAMETERS
}

cleanup() {
  echo "\n*** Performing cleanup, please wait ***\n"
  
    runningANTSpids=$( ps --ppid $$ -o pid= )
  
  for thePID in $runningANTSpids
  do
      echo "killing:  ${thePID}"
      kill ${thePID}
  done
  
  return $?
}

control_c()
# run if user hits control-c
{
  echo -en "\n*** User pressed CTRL + C ***\n"
  cleanup
  exit $?
  echo -en "\n*** Script cancelled by user ***\n"
}

#Provide output for Help ]}'
if [[ "$1" == "-h" || $# -eq 0 ]];
    then 
        Help >$2
fi

############
#DEFAULT VALUES
############
DIM=3
FIXEDIMAGE=()
MOVINGIMAGES=()
OUTPUNAME=output_oct
NUMBEROFTHREADS=1
SPLINEDISTANCE=26
PRECISIONTYPE='d'
CCRADIUS=4
MASK=0
USEHISTOGRAMMATCHING=0
OUTPUTFOLDER=''
############
# Reading command line argument
############
while getopts "d:f:h:m:j:n:p:r:s:x:o:" OPT
    do
    case $OPT in
        d) #dimension
    DIM=$OPTARG
    ;;
        f) #fixed image
    FIXEDIMAGE[${#FIXEDIMAGE[@]}]=$OPTARG
    ;;
        h) #help
    Help
    exit 0
    ;;
        m) #moving imageS
    MOVINGIMAGES[${MOVINGIMAGES[@]}]=$OPTARG
    ;;
        j) #histogram matching
    USEHISTOGRAMMATCHING=$OPTARG
    ;;
        n) #number of threads
    NUMBEROFTHREADS=$OPTARG
    ;;
        p) #Preceision type
    PRECISIONTYPE=$OPTARG
    ;;
        r) # cc radius
    CCRADIUS=$OPTARG
    ;;
      s)  # spline distance
   SPLINEDISTANCE=$OPTARG
   ;;
        x) #inclusive mask
    MASK=$OPTARG
    ;;
        o) #output folder
    OUTPUTFOLDER=$OPTARG
    ;;
      \?) #getopts issues qn error message
      echo"$USAGE" >&2
      exit 1
      ;;
    esac
done
        
#############
#CHECK INPUT
#############
if [[ ${#FIXEDIMAGE[@]} -ne 1 ]];
    then 
        echo " Number of fixed image is not equal to 1 " 
        exit 1
fi

if [[ ${#MOVINGIMAGES[@]} -lt 1 ]];
    then 
        echo " Number of moving images should be equal or greater than 1"
        exit 1
fi

if [[ ! -f "${FIXEDIMAGE[0]}" ]];
    then
        echo "Fixed image '${FIXEDIMAGE[0]}' does not exist. See usage e'$0 -h 1' "
        exit 1
fi

for (( i=0 ; i < ${#MOVINGIMAGES[$@]} ; i++ ))
  do 
    if [[ ! -f "${MOVINGIMAGES[$i]}" ]];
      then 
        echo "Moving image '${MOVINGIMAGES[$i]}' Does not exist. See Usage '$0 -h 1' "
        exit 1
    fi
done


############
# Set numnber of threads
############
ORIGINALNUMBEROFTHREADS=${ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS}
ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=$NUMBEROFTHREADS
export ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS

############
# Print out options
############
reportMappingParameters

###########
# Infer the number of levels based on 
# The sie of the input fixed image
###########
ISLARGEIMAGE=0

SIZESTRING=$( ${ANTSPATH}/PrintHeader ${FIXEDIMAGE[0]} 2 )
SIZESTRING="${SIZESTRING%\\n}"
SIZE=( `echo $SIZESTRING | tr 'x' ' '` )

for (( i=0; i<${#SIZE[@]}; i++ ))
  do
    if [[ ${SIZE[$i]} -gt 256 ]];
      then
        ISLARGEIMAGE=1
        break
      fi
  done
  
  
###########
#Construct Mapping Stage
###########
RIGIDCONVERGENCE="[1000x500x250x0,1e-6,10]"
RIGIDSHRINKFACTORS="8x4x2x1"
RIGIDSMOOTHINGSIGMAS="3x2x1x0vox"

AFFINECONVERGENCE="[1000x500x250x0,1e-6,10]"
AFFINESHRINKFACTORS="8x4x2x1"
AFFINESMOOTHINGSIGMAS="3x2x1x0vox"

SYNCONVERGENCE="[100x70x50x0,1e-6,10]"
SYNSHRINKFACTORS="8x4x2x1"
SYNSMOOTHINGSIGMAS="3x2x1x0vox"

if [[ $ISLARGEIMAGE -eq 1 ]];
  then
    RIGIDCONVERGENCE="[1000x500x250x0,1e-6,10]"
    RIGIDSHRINKFACTORS="12x8x4x2"
    RIGIDSMOOTHINGSIGMAS="4x3x2x1vox"

    AFFINECONVERGENCE="[1000x500x250x0,1e-6,10]"
    AFFINESHRINKFACTORS="12x8x4x2"
    AFFINESMOOTHINGSIGMAS="4x3x2x1vox"

    SYNCONVERGENCE="[100x100x70x50x0,1e-6,10]"
    SYNSHRINKFACTORS="10x6x4x2x1"
    SYNSMOOTHINGSIGMAS="5x3x2x1x0vox"
  fi



#########TRANSLATION REGISTRATIONS####################
for(( i=0; i<${#MOVINGIMAGES[@]}; i++ ))
do
    tx=Translation
    INITIALSTAGE="--initial-moving-transform [${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1]"
    RIGIDSTAGE="--transform ${tx}[0.1] \
                --metric MI[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,32,Regular,0.25] \
                --convergence $RIGIDCONVERGENCE \
                --shrink-factors $RIGIDSHRINKFACTORS \
                --smoothing-sigmas $RIGIDSMOOTHINGSIGMAS"
    STAGES="$INITIALSTAGE $RIGIDSTAGE"
    OUTPUTNAME="../result/Translation/${i+4}to1"
    echo "##########################################################################################"
    echo "#############Registration for '${MOVINGIMAGES[$i]}' to target '${FIXEDIMAGE[0]}'###########"
    echo "##################Initial stage of registration - Translation Only #######################" 
    COMMAND="${ANTS} --verbose 1\
                     --dimensionality $DIM $PRECISION \
                     --output [$OUTPUTNAME,${OUTPUTNAME}_translation_Warped.nii.gz,${OUTPUTNAME}_translation_InversedWarped.nii.gz] \
                     --interpolation Linear
                     --use-histogram-matching ${USEHISTOGRAMMATCHING} \
                     --winsorize-image-intensities [0.005, 0.995] \
                     ${MASK} \
                     $STAGES"
    echo " antsRegistration call:"
    echo "--------------------------------------------------------------------------------------"
    echo ${COMMAND}
    echo "--------------------------------------------------------------------------------------"
    echo "Done"
    
    $COMMAND
done



#########RIGID REGISTRATIONS####################
for(( i=0; i<${#MOVINGIMAGES[@]}; i++ ))
do
    tx=Rigid
    INITIALSTAGE="--initial-moving-transform [${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1]"
    RIGIDSTAGE="--transform ${tx}[0.1] \
                --metric MI[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,32,Regular,0.25] \
                --convergence $RIGIDCONVERGENCE \
                --shrink-factors $RIGIDSHRINKFACTORS \
                --smoothing-sigmas $RIGIDSMOOTHINGSIGMAS"
    STAGES="$INITIALSTAGE $RIGIDSTAGE"

    OUTPUTNAME="../result/Rigid/${i+4}to1"
     echo "##########################################################################################"
    echo "Registration for '${MOVINGIMAGES[$i]}' to target '${FIXEDIMAGE[0]}'"
    echo "Rigid Registration Only " 
    COMMAND="${ANTS} --verbose 1\
                     --dimensionality $DIM $PRECISION \
                     --output [$OUTPUTNAME,${OUTPUTNAME}_Rigid_Warped.nii.gz,${OUTPUTNAME}_Rigid_InversedWarped.nii.gz] \
                     --interpolation Linear
                     --use-histogram-matching ${USEHISTOGRAMMATCHING} \
                     --winsorize-image-intensities [0.005, 0.995] \
                     ${MASK} \
                     $STAGES"
    echo " antsRegistration call:"
    echo "--------------------------------------------------------------------------------------"
    echo ${COMMAND}
    echo "--------------------------------------------------------------------------------------"
    echo "Done"
    
    $COMMAND
done

#########AFFINE REGISTRATIONS####################
for(( i=0; i<${#MOVINGIMAGES[@]}; i++ ))
do
    tx=Rigid
    INITIALSTAGE="--initial-moving-transform [${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1]"
    RIGIDSTAGE="--transform ${tx}[0.1] \
                --metric MI[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,32,Regular,0.25] \
                --convergence $RIGIDCONVERGENCE \
                --shrink-factors $RIGIDSHRINKFACTORS \
                --smoothing-sigmas $RIGIDSMOOTHINGSIGMAS"
    AFFINESTAGE="--transform Affine[0.1] \
             --metric MI[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,32,Regular,0.25] \
             --convergence $AFFINECONVERGENCE \
             --shrink-factors $AFFINESHRINKFACTORS \
             --smoothing-sigmas $AFFINESMOOTHINGSIGMAS"
    STAGES="$INITIALSTAGE $RIGIDSTAGE $AFFINESTAGE"
    
    OUTPUTNAME="../result/Affine/${i+4}to1"
     echo "##########################################################################################"
    echo "Registration for '${MOVINGIMAGES[$i]}' to target '${FIXEDIMAGE[0]}'"
    echo "#######################Affine Registration#####################" 
    COMMAND="${ANTS} --verbose 1\
                     --dimensionality $DIM $PRECISION \
                     --output [$OUTPUTNAME,${OUTPUTNAME}_Affine_Warped.nii.gz,${OUTPUTNAME}_Affine_InversedWarped.nii.gz] \
                     --interpolation Linear
                     --use-histogram-matching ${USEHISTOGRAMMATCHING} \
                     --winsorize-image-intensities [0.005, 0.995] \
                     ${MASK} \
                     $STAGES"
    echo " antsRegistration call:"
    echo "--------------------------------------------------------------------------------------"
    echo ${COMMAND}
    echo "--------------------------------------------------------------------------------------"
    echo "Done"
    
    $COMMAND
done

#########Diffeomorphic REGISTRATIONS####################
for(( i=0; i<${#MOVINGIMAGES[@]}; i++ ))
do
    tx=Rigid
    INITIALSTAGE="--initial-moving-transform [${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1]"
    RIGIDSTAGE="--transform ${tx}[0.1] \
                --metric MI[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,32,Regular,0.25] \
                --convergence $RIGIDCONVERGENCE \
                --shrink-factors $RIGIDSHRINKFACTORS \
                --smoothing-sigmas $RIGIDSMOOTHINGSIGMAS"
    AFFINESTAGE="--transform Affine[0.1] \
             --metric MI[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,32,Regular,0.25] \
             --convergence $AFFINECONVERGENCE \
             --shrink-factors $AFFINESHRINKFACTORS \
             --smoothing-sigmas $AFFINESMOOTHINGSIGMAS"
             
    SYNMETRICS="$SYNMETRICS --metric CC[${FIXEDIMAGE[0]},${MOVINGIMAGES[$i]},1,${CCRADIUS}]"
    SYNSTAGE="${SYNMETRICS} \
            --convergence $SYNCONVERGENCE \
            --shrink-factors $SYNSHRINKFACTORS \
            --smoothing-sigmas $SYNSMOOTHINGSIGMAS"
    SYNSTAGE="--transform BSplineSyN[0.1,${SPLINEDISTANCE},0,3] \
             $SYNSTAGE"
    STAGES="$INITIALSTAGE $RIGIDSTAGE $SYNSTAGE"
     
    OUTPUTNAME="../result/Diffeomorphic/${i+4}to1"
     echo "##########################################################################################"
    echo "Registration for '${MOVINGIMAGES[$i]}' to target '${FIXEDIMAGE[0]}'"
    echo "#######################Diffeomorphic Registration#####################" 
    COMMAND="${ANTS} --verbose 1\
                     --dimensionality $DIM $PRECISION \
                     --output [$OUTPUTNAME,${OUTPUTNAME}_SyN_Warped.nii.gz,${OUTPUTNAME}_SyN_InversedWarped.nii.gz] \
                     --interpolation Linear
                     --use-histogram-matching ${USEHISTOGRAMMATCHING} \
                     --winsorize-image-intensities [0.005, 0.995] \
                     ${MASK} \
                     $STAGES"
    echo " antsRegistration call:"
    echo "--------------------------------------------------------------------------------------"
    echo ${COMMAND}
    echo "--------------------------------------------------------------------------------------"
    echo "Done"
    
    $COMMAND
done



###########
# Restore original nuber of threads
###########
ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=$ORIGINALNUMBEROFTHREADS
export ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS
