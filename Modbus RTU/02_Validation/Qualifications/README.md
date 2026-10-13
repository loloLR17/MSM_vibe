# Qualifications des jalons TR2 — A-01

Ce répertoire contient les fiches synthétiques de qualification **indépendante par ChatGPT**. Il ne remplace ni les rapports Codex ni les traces de tests. Les fiches portent uniquement sur les critères démontrés.

## Convention
- Une mission `DONE` n'est pas automatiquement un jalon qualifié.
- Une fiche `QUALIFICATION_Mnnn.md` décrit le périmètre, le statut, le SHA de code, la toolchain et les options de build, l'empreinte binaire, le matériel, les essais, les preuves, les limites et la décision datée de ChatGPT.
- Après revue positive, tag Git **annoté** `tr2/Mnnn` sur le SHA du **code réellement testé**, distinct du commit documentaire ; vérifier la publication distante.
- Si les preuves sont incomplètes, indiquer `REVUE EN ATTENTE` ou `NON QUALIFIÉ`, **sans tag**.
- Aucun tag historique automatique et aucune assimilation des tests hôte à des essais physiques.

## Première revue
M015 est le premier candidat : code rapporté `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae`, preuves documentaires publiées ultérieurement. Le contrôle indépendant reste à réaliser avant toute décision ou tag.
