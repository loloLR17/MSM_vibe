# Complément terrain utilisateur — écart de séparation CN8/CN9 versus CN7/CN10

Date : 2026-11-05. Mission active : `TR2-20261105-SHIELD-V19-C05-OFFICIAL-CAD`.

## Observation, à traiter comme hypothèse et non décalage prouvé

Sur le **gabarit papier imprimé correctement à l'échelle 100 mm**, l'utilisateur identifie une cause plausible du défaut d'alignement : l'espace entre **CN8 et CN9** sur un flanc et entre **CN7 et CN10** sur l'autre. Le dessin imprimé lui montre **environ 5 mm dans les deux cas** ; sur sa Nucleo réelle il constate **environ 5 mm d'un côté et 4 mm de l'autre**. L'utilisateur n'a **pas encore précisé quelle paire possède réellement les 4 mm**, ni si la cote correspond au vide entre corps plastiques, aux broches extrêmes ou à un autre bord de référence. Écart supposé `~1 mm`, pas valeur de correction directe. Ne pas redemander une série de mesures point-à-point : priorité aux fichiers constructeur.

## Vérification sur les coordonnées actuelles du PCB TR2

Dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/docs/comparaison-C05.csv` :
- J2 / CN8 dernier centre (même colonne) : Y=52,94 mm ; J3 / CN9 premier centre : Y=58,02 mm ; **écart entre centres = 5,08 mm**.
- J1 / CN7 dernier centre : Y=47,86 mm ; J4 / CN10 premier centre : Y=52,94 mm ; **écart entre centres = 5,08 mm**.
- Ces valeurs sont **issues de l'implantation KiCad actuellement reconstruite depuis UM2861**, pas d'une position officielle ST C05 confirmée. Elles ne représentent pas nécessairement un vide entre corps de connecteurs. Les colonnes `ST_C05_*_mm` restent vides.

## Consigne V1.9 à Codex

Tester **d'abord cette dissymétrie** lors de l'analyse des fichiers officiels ST MB1549-U575ZIQ-C05, et vérifier si un des connecteurs CN9 ou CN10, ou un groupe entier, a été placé à mauvais Y. Contrôler aussi l'origine, les axes, un éventuel miroir et les pièces plastiques. Ne pas réduire les incertitudes à « décaler de 1 mm », et ne déplacer aucune empreinte sans cote métrique C05 vérifiée. Corriger au minimum nécessaire avec tests PCB/schéma/CAM et nouveau gabarit. Le constat matériel à barre100mm rend une simple erreur globale d'échelle peu probable, mais l'hypothèse reste à confirmer.

Ce complément **ne modifie pas la mission READY active pendant son exécution**, ni la conception ni les rapports existants. **STOP fabrication** jusqu'à validation.
