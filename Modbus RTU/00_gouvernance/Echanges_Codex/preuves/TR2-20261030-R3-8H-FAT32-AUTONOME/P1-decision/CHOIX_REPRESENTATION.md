# P1 — décision de représentation requise, pas de correctif métrologique implicite

Mission R3-8H, addendum6bc4f24b §P1-6. État : **analyse**, aucun nouveau code/format ni essai. P0 réellement relu au PC puis depuis main1991132b : données utiles intactes, sortie76473993 clippée avant le framing SD. Source brute de cet instant absente. Les causes mouvement/capteur/SPI restent indéterminées ; élargir un format ne résout pas une éventuelle entrée fausse.

## Faits du code et preuves réutilisées

`fir_decimator10.c` accumule déjà les107 produits dansint32, arronditQ15 puis effectue le clamp versint16 et lèveOUTPUT_CLIP. Sa somme absolue47468 borne l'accumulateur à1555431424<INT32_MAX. Le résultat non réduit peut atteindre47468comptes (borne conservatrice), soit11,582192g **nominaux calculés**, tandis que le capteur reste physiquement configuré à±8g. Une amplitude calculée horsint16 après convolution ne prouve donc pas une entrée capteur hors±8g.

Le test existant `test_fir_decimator10.c::constant_and_rails` vérifie DC, rails et ringing sur échelon±32760, avec clip sansSOURCE_RAIL. Le reproducteurC déjà publié confirme47465avant clamp. Ils ne sont pas rejoués. L'oracle préfixe103 sorties reste exact mais ne couvre pas la source de l'incident ; aucune prétendue reconstruction des107 bruts à partir des78 sorties d'un record.

Le formatv3 contient6B/XYZ,244µg/compte,78XYZ/record. Les X/Y réels à−32768 peuvent avoir perdu une amplitude non récupérable ; zéroSOURCE_RAIL n'annule pasOUTPUT_CLIP. Le masque précis des axes clippés n'est pas archivé. La correspondanceSHA amontRAM/SD exclut une modification de ces octets introduite par le stockage, pas une anomalie amont.

## Choix à trancher explicitement par ChatGPT/utilisateur

1. **Conserver v3 et sa qualité stricte.** Le premier essai reste rejeté et le gardebc0f reste nécessaire. Une deuxième campagne identique ne corrigerait aucun mécanisme ; la cause réelle reste à instrumenter, avec décision de portée avant de consommer l'unique nouveau8H conditionnel. Aucun PASS garanti.
2. **Autoriser une représentation expérimentale élargie et versionnée**, par exempleXYZint32 à244µg/compte avec même FIR/gain1 et même capteur±8g. Préserver les flags/compteurs de source et erreurs ; distinguer explicitement le passage de l'ancienne limiteint16 d'un vrai clamp de la nouvelle représentation. Le traitement d'un dépassement de l'ancienne limite dans la qualité/acceptation doit être décidé, **pas supprimé pour obtenir PASS**. Ne changer ni le profilV1 gelé, ni B3/AC1, ni la sensibilité, ni les fichiersv1/v2/v3 historiques. Nouveau lecteur/format expérimental seulement après cette décision.

Int32 est une proposition simple pour le calcul et la lecture ; un packing24bits pourrait préserver la même précision avec moins d'octets mais ajoute un encodage. Ce choix ne doit pas être réalisé implicitement. Comparaison arithmétique pour **le même nombre observé de80867920XYZ**, en supposant l'enveloppe actuelle44B+charge dans records512B ; **pas un schéma v4 adopté ni une préallocation validée** :

| Représentation | B/XYZ | XYZ/record | Données engagées avec header/footer |
|---|---:|---:|---:|
| v3int16 existant |6|78|530826752B|
| éventuel int24 |9|52|796239360B|
| éventuel int32 |12|39|1061651968B|

Formule `(ceil(N/capacité)+2)*512`. Les trois restent sous4GiB pour ceN, sans que cela démontre la borne de taux maximal, la marge d'allocation, la contiguïté ni les performances futures. Le trafic double enint32 : **pas de promesse de tenue temps réel**. File64KiB et quanta/gardes restent protégés ; modèles de flux/SD, tests numériques pertinents, rétrocompatibilité et nouveau court gate physique devront passer avant tout8H.

## Ce qui est exclu sans autre décision

Réduire gain/précision, modifier coefficients/bande utile, passer capteur±16g, réinterpréterv3, retirerOUTPUT_CLIP ou accepter rétroactivementE8N0005. L'accumulateur n'est pas en overflow démontré ; le rendreint64 ne suffirait pas à supprimer le clampint16. Le correctifbc0f règle uniquement le faux footerCOMPLETE ; aucune solution à la cause physique n'est encore qualifiée.

## Arrêt réel et reprise

**BLOCKED sur décision structuranteP1-6**, pas sur quota ni geste physique actuel. TR2-002 hors tension/microSD au PC selon utilisateur ; P0 et originaux conservés. Pas de secondSTART, flash/reset ou nouvelle demande de réinsertion avant choix et préparation validée. Après décision : modifier uniquement le profil expérimental autorisé, tests nécessaires, court gate puis unique second8H si tous critères satisfaits, sous le même ID et avec noms neufs. La sessionCodex doit alors se terminer après checkpointSTART comme au premier essai.
