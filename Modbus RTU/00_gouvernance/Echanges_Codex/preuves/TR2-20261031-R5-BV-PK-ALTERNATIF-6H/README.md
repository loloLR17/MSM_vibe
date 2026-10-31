# R5-PK — six heures réelles, résultats candidats et réserves

[Pagehorsligne](index.html), [synthèse1page](SYNTHESE_POC.md),
[accélération1–5Hz](BV_PK_ACC_1_5.png), [vitesse5–100Hz](BV_PK_VEL_5_100.png).
[CSV30804composantes10/60s](components.csv.gz), [méthodes](methods.json),
[contrôlestemporels](independent_real.json), [sensibilitédurée](window_sensitivity.csv),
[sensibilitéhorloge](clock_sensitivity.csv), [SQLite](sqlite-proof.json).
[Méthodedétaillée](../../../../../experiments/r5_bv_pk/METHOD.md).

**BV_PK_CANDIDATE ; comparaison globale de la paire INDETERMINATE.**
Les chiffresdescasrejetés sontcandidats, pas desmesures deconfort qualifiées.
Enaccélération,0fenêtre-axerecevable. Envitesse,11fenêtres-axes parhypothèse
(Y1/Z10) recevables sousconditions ; aucune minute ne l'est surles3axes.
La figurevitesse présente uniquementcescomparaisons partielles ; aucunegraduation
BV n'est superposée auxpointsaccélération nonrecevables. Le tableaucontextuel
rappellelesrepères sanslesappliquer auxcas indéterminés. Aucunsigne «favorable».

SourceprimaireBVjuillet2026 vérifiée surtexteetimagesPDFlocales : CF1,
sec3Tab5vitesse4/4.5/5mm/speak ; Tab6accélération125/141/157mm/s²peak.
CorrespondancePCmachineoccupéenmer àconfirmer, axesnavire inconnus. Nerepas
confondre avecTab4RMSpondéré1–80Hz ni anciensRMSnonpondérés. LesPDFprotégés et
capturescompletes restentlocaux ; références/empreinte dansreferences.json.

Hannpériodique/correctioncohérente+horsbin, amplitudederaiesséparées ;
vitessecalculée àlafréquencedelacomposante. Sousfenêtres10s etfenêtre60s, pas
√2duRMSbroadband. Maxdespicscourts≠MRAgaranti sousmodulation. Tolérancesfixées :
sinus1%/0.002Hz, ajustementindépendant1%/1e-5Hz connus ; réels5%/0.02Hz et5%
surle10s retenu. Lesrejets ne sont pas supprimés pourforcerunecomparaison.

Troisplages faibles/médianes/fortes parbande contrôlées et2horloges :
18comparaisons60s (12initiales et6surlesextrêmesglobauxdelavitesse),
6recoupements10s recevables. Enaccélération, écarts11.50%,
0.41%,31.00% ; deuxécarts hors5%, statutsdéjà nonrecevables. Envitesse, écarts60s
0.83–1.25% et10s1.70–2.37%, dansleslimites. Ce n'est pas unétalonnagephysique.

Forte sensibilité auxdurées : maxdescandidats10s/estimation60s, médianes2.23
(accélération) et1.77(vitesse), P953.41/2.94. Lespicsderang1 peuventchanger
source ; cesratios neconvertissent pas une grandeur versBV. DonnéesCSV gardées.
Les fréquences sontestimées, pasSI ; deuxbasesMCU/QPC, mêmesfenêtresMCU,
UTC_UNKNOWN, pasd'arrivée/régimeattribué. Source/gravity/DC originaleconservée.

Lescontrôlescomplémentaires ne répètentpas ceuxdéjàeffectués ; ils couvrent
lescandidatsvitessefaible/médian/fortdelacampagneentière, même nonrecevables.
Leursécarts0.31–1.20% n’annulentpas leursstatutsmodulés/nonrésolus.

Schéma3 = **copiemigrée** ducataloguev2, originauxv1/v2 inchangés. Lescinqtables
scientifiquesantérieures (toutesleurslignes empreintées) sontidentiquesavant/après.
Unseulcampaign_sha ;4anciennesanalyses/2204RMS+4nouvellesPK/30804composantes,
une référenceversionnée,18contrôles ; deuximportsidem/intégrité/FK/4requêtesPASS.
Requêtes groupées paranalyse, jamaisdesversionsmélangées. PasdeXYZbrut enSQL.
V3locale48.43MB nonpubliée, pasd'uploadduTR2/ducheckpointd'agrégats ; chemins/SHA
précis dansLOCAL_ARTIFACTS.json etsqlite-proof.json. Futureunification : lecturev3
pourPK+1–80, v1pour2–25/PSD, jointureparcampaignSHA/analysisID sansdupliquerXYZ.

Calculréelréussi100.45s/picRSS375256KiB≈366.5MiB, mémoirebornée parminutes/lots.
Premiercalcul arrivéàl'exportmaisnonconservé àcausede boolNumPy ; reprisePCjustifiée,
conversiontestée etcheckpointd'agrégatssauvéavantcontrôles ajoutés. Échecsconservés.
**11testsPASS** ; aucunancientestRMS/calculintégralprécédentrelancé. Lesrendus et
sensibilités sont dérivés desexports, sansnouvelleFFT ni physique. Sources/waveforms/
FIR/firmware/DBLIVE/worktrees/stashespréservés ; aucuneopérationSTM32/SD/COM.

R3CLOSED_WITH_RESERVATIONS maintenue : digestRAMindépendant perdu, intégritéinterne
seule reconfirmée. Aucungrade, conformité, expositionA(8), causalitémachine,
santé oustructuredéduits. RevueindépendanteChatGPT suivante ; décision surméthode
d'extraction répétitive dessignauxmodulés avantinterprétationPKglobale. Wmreste
unautrechantier et n'estpas utilisé commeblocagedeprincipe decetteanalysePK.
