# R5-PK — pics de composantes, PC uniquement

[Définition complète, sources primaires et limites](METHOD.md).
Code isolé, lecteur strict R3 et décodage parblocs R5 réutilisés. AucunWm,
aucun nouveau RMSlarge-bande1–80Hz, aucun accèsmatériel.

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -m unittest discover -s experiments/r5_bv_pk -p 'test_*.py' -v
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_bv_pk/run.py /home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2 /chemin/PC/nouveau-dossier
```

La sortie doit être nouvelle. OrigineimmuableSHA vérifiéeavant/après etCRC/index/
session/footer parlecteurstrict. Mémoirebornée : uneminute deXYZ etdeslots4096records ;
les sorties sont des composantes agrégées, jamais58552136objetsPython.

Tolérances avantexécution : sinusisolé1% amplitude/0.002Hz, ajustementtemporel
indépendant1% et1e-5Hz sursinusconnu ; surleréel5% et0.02Hz, puis5% surle10s
portantlemaximum retenu. Échecs decontrôle recevable bloquent la catégorie.
Les paramètres destationnarité/contraste/forme/bords sont déclarés dansMETHOD.md.

Deuxfamilles de résultats etdecomposantes distinctes : accélérationcrête1–5Hz
(mm/s²) et vitessecrête5–100Hz(mm/s), CF=1 parcomposante. Les absences et
invalidités sont NULL/vides, pas0. Le statutquasi-stationnaire permet unecomparaison
indicative sousconditions, jamais uneclasseattribuée. Peak-holdspectral≠MRAgaranti.
Les fréquences sont estimées selonMCU/QPC ; axes etmontage nonétalonnés,
UTC_UNKNOWN ; réservedigestRAM indépendante inchangée.

Métrologieauxiliaire : PDFBV téléchargédepuislesiteofficiel etrendu localement
pourlecture desformules/tableaux ; outilPDF installé uniquement dansunrépertoire
cacheprivé séparé, sans toucherle venvscientifique R5 oulesparamètresglobaux.
AucunPDFcomplet/capture de pagesprotégées nefigure dansleslivrablespublics.
Aucunbuildfirmware/.NET ni reprise desancienstestsinchangés : validationcibléePC.
