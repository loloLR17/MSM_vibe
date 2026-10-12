# Preuves M015-O

Code évalué906de1ad85f6b3d7d6737bc699f29a482590614d ;la vraie fonction ST est
compilée depuis référence pinnée,driverSTf83 explicitementdistinctdugitlinkd995
inaccessible. Sourcevendor inchangée ;patches décriventformats/buffer/plateforme.
Copiesgénérées sontnormalisées LF/sansespacesfinaux ;patch-original.hex conserve
lesoctets du patchapplicable,originauxIOC/READMEencodésexacts disponiblesenhex.

Trois campagnesconsommées :official(mode3/getterintact),level10(mode3/aliaslocal),
mode0(mêmealias/seulSPIchangé). AucunXYZ ;confirmationimagepréparée nonflashée.
Les *.bin.hex se reconstruisent via bytes.fromhex ;images et readbacksdes seuls
bytesprogrammés, pasbackupFlash2MiB. ABIResult4040/Trace40/Preclose76 little-endian.
Words[]deBenchResult estBSS,inutilisé :donnéesmesurées dansofficial-words.bin.hex.
ConsoleST réelle dansofficial-text.bin.hex ;label[ms]originalmaiscompteurbrut.
status10/polls/st_batch=0 dansResult ne représententpaslesprofilsougetterBDR,
car lemainSTne lesrenseignepas ;voircompiler,qualification ettransactions.

Photos/capturewire nonobtenues ;résultatsMCU only. LogsnonUTF8 decodecp1252
avec*.original.hexpréservantlesoctets ;autreslogs normalisésLF. ManifestSHA,
preservation.json et comparaisonmode3/mode0 conservés. Ne pasrelancerlesscripts
consommés :cheminsexclusifs,quotalimitedeuxvariantes épuisé. Capteurau repos,
firmwarefinalmode0inactif,aucunTR2principal/média/Modbus/HP.
