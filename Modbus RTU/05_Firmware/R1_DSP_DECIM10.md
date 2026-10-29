# R1-DSP-A — FIR causal XYZ /10, version expérimentale1

Mission `TR2-20261029-R1-DSP-DECIM10-HOST`, référentiel545b96e1. Livraison **logicielle uniquement**, sans intégration FIFO, flash ou debug. Ne remplace ni AC1 ni les blocs normatifs B3/B4/B5 ; ne définit pas le format archive final V1. Les0–700Hz sont conservés avec le DC. La zone de sortie700–1333Hz n’est pas une bande garantie.

## Architecture et interface

`include/tr2/domain/acquisition/fir_decimator10.h` et `src/domain/acquisition/fir_decimator10.c` constituent un propriétaire d’état fourni par l’appelant, sans allocation dynamique, I/O ou travail en ISR. Les XYZ sont des **comptes int16**, pas des mg. Un même jeu de coefficients est utilisé pour les trois axes. Les dernières107entrées sont conservées en anneau ; les321MAC ne sont effectuées qu’une fois par10sources.

Audit du chemin existant : `FifoLiveConsume` et `live_fifo_acquisition_bind_event_metrology` ne lisent pas un deuxième capteur. `fifo_event_time_consume` vérifie les tags/index, attend le prochain anchor et transmet les8XYZ au calcul LIVE via `live_metrology_sample`. Un futur dispatch doit raccorder cette brique **sur ces mêmes XYZ validés**, dans le foreground, sans ajouter de lecteurSPI3 ni de dates synthétiques. Aucun de ces chemins existants n’est modifié ici.

- Initialiser, puis `start` avec ID de segment strictement croissant, index source d’origine et fréquence rationnelle. Émission aux index relatifs0,10,20… à l’origine de segment, indépendante de la découpe des appels. Une origine absolue non nulle est permise et explicitement conservée.
- `push` consomme un XYZ ; `process` consomme une séquence sans allocation et rend les comptes exacts `consumed/written`, y compris en cas de faute. Les tableaux ne doivent pas se chevaucher. Capacité de sortie insuffisante : `NOT_AVAILABLE` **avant** la prochaine entrée émettrice ; reprise possible au même élément sans perte ni décalage.
- `stop` ne produit rien : exactement`ceil(N/10)` sorties pourNentrées du segment, y comprisN=0 ; aucune queue de convolution n’est présentée comme des mesures supplémentaires. Le zéro initial est un état mathématique du FIR, signalé parWARMUP pendant les11premières sorties (n=0..100) ; première fenêtre complète émise àn=110.
- Toute entrée invalide, rupture d’index, disponibilité temporelle changée ou temps non strictement croissant ferme le segment avant consommation du fautif. Le caller doit aussi appeler`invalidate` sur perte FIFO/tag/anchor/overrun détectée en amont. Pas de réparation implicite ni de reprise automatique ; le prochain segment réinitialise l’historique et la phase. Une borne entière atteinte est signalée, jamais enroulée.
- Temps fourni : microsecondes monotones **déjà déroulées** par l’autorité. Le composant ne transforme pas un compteur brut32bits en date. La sortie conserve le temps de l’extrémité causale, et **pas** celui du centre du filtre ; le délai53sources vaut1,9875ms seulement à l’ODR nominale. AucunUTC n’est fabriqué. Temps absent : flag explicite et champ0, sans déduction depuis l’index.
- Cadence : rationnel externe`source_num/source_den`, sortie`source_num/(10*source_den)`, avec dénominateur contrôlé contre le débordement. Le booléen`measured` décrit la provenance donnée par le caller ; la brique ne mesure pas elle-même une ODR. Les fixtures de dérive ne constituent pas des observations capteur.

## Arithmétique choisie et qualité

Génération déterministe par SciPy1.17.0/NumPy2.3.5 : `firwin(107,1333.3333333333333,fs=80000/3,window=('kaiser',7.85726))`. Le script versionné `tests/dsp/generate_decim10.py` est exécuté explicitement, jamais au build. Les coefficients Q15 sont arrondis nearest-even, puis le coefficient central est réduit de5unitésQ15 pour rendre la somme **exactement32768**, sans casser la symétrie. Cette correction fait partie de la version1 et de ses empreintes ; ce ne sont pas les coefficients Q15 indépendamment arrondis du tableau historique.

| Voie réellement testée en C | Écart maximal observé à l’oracle float64 int16, jeux publiés | Borne continue du rejet des coefficients | Choix |
|---|---:|---:|---|
| Q15 exactDC |25comptes (cas adversarial), borne générale27 |−66,538dB | Retenue : MAC32bits,214B de coefficients |
| Q31 exactDC, candidate test |0compte |−77,006dB | Plus précise ; coefficients428B et accumulateurs64bits |
| float32, candidate test |1compte |−77,006dB pour les coefficients | Le build actuel utilise des appels logiciels pour convertir/multiplier/additionner |

La tolérance Q15 est définie **avant les rejeux** par`ceil(32768*sum(abs(hQ15-h64))+1)=27`comptes, incluant les deux arrondis. La borne d’accumulateur est`47468*32768=1555431424`, inférieure àINT32_MAX pour **tout** historique int16 et toutes ses sommes partielles ; assertion statique présente. Arrondi signé nearest-even sans décalage signé dépendant de l’implémentation, puis clamp explicite. Ce compromis numérique respecte les portes de cette mission ; une précision ultérieure plus stricte pourra justifier Q31, sans supposer un gain CPU non mesuré.

La source aux codes rails est signalée séparément : masques par axe dans l’historique contributif et compteur de XYZ source touchant au moins un rail. Un dépassement du FIR marque le masque d’axes écrêtés, `OUTPUT_CLIP` et un compteur de **sortiesXYZ** affectées. L’échelon32760→−32760 provoque6sorties écrêtées **sans aucun code source aux rails**. Les compteurs de diagnostic saturent avec flag explicite ; les index/comptes structurants ne peuvent pas boucler.

Les alternatives sont des kernels de test, sans intégration produit. Elles partagent l’historique validé pour comparer le calcul, puis sont comparées à la convolution SciPy indépendante. Leurs métriques qualité dans le replay restent celles de l’état **Q15 de référence**, pas une qualification séparée de lifecycle float32/Q31 ; ce périmètre est écrit dans chaque ligne du manifeste.

## Validation numérique reproductible

`tests/dsp/qualify_decim10.py` régénère le FIR float64 indépendamment du kernelC et utilise `scipy.signal.upfirdn`, tronqué à`ceil(N/10)`. Il compare **59rejeuxC réels**, pas des sorties simulées à la place du composant. Q15 etQ31 doivent correspondre exactement à leur oracle quantifié ; tolérancefloat32≤1compte. Les trois SHAsfloat64 externes6s sont reproduits :

- DC : `7a90f77cc609e017be51c46ae4d9cac3d8476bdf2abe396742da5c504341a828`.
- Impulsion : `27f14a2cb1ff2d01bc9df168a1cabb756f1419b1ad78903a72aa416b28e14995`.
- Multisinusoïdes : `56696f8d06264bcb24152ec8b705aa209056309b194c1853c9c48dbb5e4e093d`.

Chaque témoin :160000sources,16000XYZ,96000octets little-endian. Les SHAsQ15 ne sont pas exigés identiques au float64 ; leurs écarts sont mesurés. Chunks1/7/13/31/255/512/1024 et pattern1/7/10/13/31/255/512/1024 produisent les mêmes octets Q15. Zéros, DC±1/rails, impulsionX isolée, tons sinus/cosinus2/5/25/100/333⅓/666⅔/700Hz, fréquences hors bande, plein format aléatoire et adversarial, segments courts/STOP/restart/gaps/invalidité/rollover sont couverts au niveau de cette couche.

Pour Q15 : erreur de gain maximale0,002766dB en0–700Hz ; pic de stopband observé−67,468dB, borne continue−66,538dB sur `[fout−700, fsource/2]`. La grille inclusive262145points est augmentée de`L*pas/2`, avec`L=(2π/fs)*sum(abs(n−53)*abs(h[n]))`, plus marge numérique1e−12 ; il s’agit d’un majorant numérique des coefficients, pas d’une certification d’arrondi IEEE ni d’un filtre analogique capteur. En ajoutant un majorant conservateur de projection tonale des arrondis entrée/sortie à amplitude12000comptes, borne−63,412dB. À petite amplitude, distinguer le plancher de quantification int16 d’un rejet relatif universel.

Les tons actualisés sont ajustés en amplitude/phase après transitoire ; 2Hz reçoit8s, soit plus de12périodes. L’alias1600→1066⅔Hz reste hors bande garantie ; son atténuation n’est pas contrainte à−60dB. Les taux rationnels externes−1000/−50/0/+50/+1000ppm conservent les portes de réponse et la relation`fout=fs/10` ; C conserve les timestamps déjà déroulés, refuse un rollover brut et change de segment sur faute.

## Build ARM et limites

Validation principale :149testsC et cross-build canoniquePASS ; testR1 enrichi de timestamps dérivés des5horloges revalidé seul, puis sousASan/UBSan. Neuf rejeux instrumentés RANDOM/STEP/ADVERSARIAL ×3arithmétiques passent. Aucune validation PC Windows n’est requise ou rejouée.

Le canonique cumulatif **ne référence pas** le FIR au runtime : nouvelle source compilée dans l’archive, retirée au linkage par GC. BIN inchangé `85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5`, text172652/data480/bss602240. Ce n’est pas le profil opérateurAC1 enFlash ni sa consommationRAM.

Le projet isolé `tests/dsp/arm` compile et linke réellement les symboles FIR avec la géométrie mémoire du linkerSTM32 existant. SesELF **n’ont pas de table de vecteurs ni de startup matériel** : ils sont uniquement des vérifications de linkage, jamais des images à flasher.

| ELF de linkage | text | data | bss (dont réserve heap/pile2560B du linker) |
|---|---:|---:|---:|
| Baseline |16|0|2568|
| Q15 avec état/entrée/sortie |2008|0|3384|
| Q15 + kernels float32/Q31 de comparaison |4000|0|3416|

Delta Q15 :1992Btext et816Bbss, dont état744B + entrée32B + sortie40B. Coefficients Q15 enFlash214B. `.su` ARM rapporte80B pour le corpspush et104B pourprocess ; la chaîne imbriquée de ces deux frames représente184B, **hors caller/libc/IRQ**. Aucun high-water de pile physique ni SRAM libre garantie n’est annoncé.

GCC14.2.1, flags effectifs`-mcpu=cortex-m33 -mthumb -O2` sans modification du profil produit ; ABIsoft par défaut confirmée par options et attributsELF, sans VFP-register args. Le désassemblage Q15 utilise`mla`, le candidatQ31`smlal`, le float32 appelle`__aeabi_i2f/__aeabi_fmul/__addsf3`. **Pas de vitesse relative Q15/Q31 déduite du seul nombre d’instructions.** Choix Q15 ici pour son empreinte et les tolérances démontrées ; performances/WCET à mesurer sur le même profil dans le lot suivant.

Le coût idéal856000MAC/sXYZ reste théorique, pas une mesure DWT. Prochaine porte R1-ARM physique distincte : appariement sans/avecFIR, durée par rafale et émission, distributions et maxima work/service/gap, pile, FIFO/IRQ/FRAM/RTU sur la cible identifiée et autorisée. Aucune SD, queuearchive ou nouveau mapping n’est ajouté ; aucune qualification de stockage, d’endurance, d’AC1 ou du produit n’est déduite.

## Commandes de reproduction

Depuis la racine dépôt, avec les dépendances Python indiquées dans un environnement isolé :

```sh
python tests/dsp/generate_decim10.py  # depuis Modbus RTU/05_Firmware
# Configurer le CMake firmware existant, puis construire test_fir_decimator10 et decim10_replay.
python tests/dsp/qualify_decim10.py /chemin/decim10_replay /chemin/preuves-neuf
python tests/dsp/check_decim10_clock.py /chemin/clock-response.json
```

Le projetARM utilise le toolchain existant `platform/stm32/cmake/arm-none-eabi-gcc.cmake`, avec `CMAKE_EXPORT_COMPILE_COMMANDS=ON`. Les commandes exactes, manifests, MAP/attributs/désassemblages, sorties et empreintes sont publiés avec [le rapport](../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md). SciPy/NumPy ne sont pas nécessaires au firmware ni aux testsC de lifecycle.
