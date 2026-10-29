# R1-DSP-A — preuves hôte et cross-build, sans cible

Code0ad417d621c0023a2c326383040a5d4e73016329. Les `.zlib` sont les octets bruts sans transformation ; BUNDLE donne SHA256 brut/stocké. Manifestes numériques, trois témoins float64 et sortie MIXQ15, commandes/versions et sources sont publiés. Les autres inputs/outputs temporaires restent dans les preuves locales et se régénèrent avec le script/seed10710 versionné ; pas de dumps massifs.

149tests complets et canoniqueARM PASS, puis testR1 enrichi sur5horloges revalidé seul/sanitizers ;59rejeuxC vsSciPy,9rejeuxASan/UBSan. Les qualités des lignes candidatesf32/Q31 sont explicitement celles de l’étatQ15, seules leurs valeurs numériques sont comparées séparément. Q15 est exact à son oracle quantifié ; ses octets n’ont pas à égaler le float64.

ELF `arm--d10_*` : **linkage seulement, sans table de vecteurs/startup appareil**. Ne pas flasher ces artefacts. AucunDWT/cycleCPU/mesure SRAM libre ou high-water physique. `canonical-*` relève du cumulatif historique, pas de l’imageAC1 laissée surcarte. D10-12/13 physiques/stockage sont différés explicitement danschecks.

EnvironnementPython isolé : NumPy2.3.5/SciPy1.17.0. `python3 -m venv` initialement indisponible fauteensurepip ; l’installation a été menée par le pip d’un environnement déjà présent vers le nouvel environnement, sans modifier Python système. Aucune interventionWindows/COM4/STM32/FRAM/SD, aucun ancien test physique rejoué.
