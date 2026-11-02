# Qualification progressive des alimentations — décision utilisateur du 02/11/2026

Mission TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR, mêmes permissions/cible001.
L'utilisateur propose de qualifier d'abord sous ST-LINK, puis CN15 avec ST-LINK,
puis CN15 seul. Après discussion du plan borné ci-dessous, il répond :
« OK nous sommes alignés, tu peux débuter quand tu veux ».
Cette instruction reprend les essais précédemment suspendus. Le contrat Git n'est
pas modifié par Codex ; ce fichier consigne l'accord humain et ses conditions.

| Palier | Alimentation et connexion | Essais conditionnels |
|---|---|---|
|1|ST-LINK CN1 seul, JP6 STLK, GDB présent avant START, sans halt en capture|2×60s B01 puis B02|
|2|CN15 secteur, JP6 USB, câble ST-LINK et GDB présents avant START|1×60s|
|3|CN15 secteur et câble ST-LINK présent, aucun GDB attaché durant ce démarrage|1×60s|
|4|CN15 secteur seul, démarrage à froid sans ST-LINK|1×60s|
|5|Configuration finale CN15 seule, uniquement après réussite des précédents|1×10min|

Arrêt au premier échec pour récupération/analyse ; pas d'enchaînement long sur échec.
Les acquisitions représentent15min si tous les paliers passent, hors initialisations,
transferts et vérifications. Aucun essai d'une heure ni8h lancé par Codex.
L'extension conditionnelle10min est expressément couverte par l'accord utilisateur
sur ce plan ; aucune déduction d'autorisation pour un essai plus long.

Référence commune : SPI3 capteur8MHz, SPI2SD4MHz comme A1C60s réussi,
±16g/488µg/v3/FIR107Q15/10, garde horloge100µs et budgets inchangés. Pas de variante
à vitesses mixtes. JP2 reste retiré pour tous les paliers ; changements de JP6 hors
alimentation. Chaque essai a un nom neuf CREATE_NEW, un départ USER volontaire
unique, sauvegarde des diagnostics et vérification intégrale des données.

B01/B02 sont des répétitions nouvelles justifiées par l'objectif de reproductibilité
et les échecs intermittents, pas une revalidation inutile d'A1C. Leurs BIN ne diffèrent
que par le littéral de nom B01/B02 (comparaison octet pour octet conservée).
Le snapshot ajoute masque exact du budget dépassé, valeurs fautives et maxima ;
la condition de refus d'origine et ses seuils restent inchangés.

Préparation logicielle : source3f803d77393c6853ed76d796367df443a0eb9eb9,
test hôte des frontières/violations simultanées PASS, ARM ciblé PASS.
Pas encore de flash ni essai B01 à ce checkpoint. Intervention demandée :
SD512 auPC, lecteurTR2vide, CN15 débranché, JP6STLK1–2, JP2retiré, ST-LINK seul.
