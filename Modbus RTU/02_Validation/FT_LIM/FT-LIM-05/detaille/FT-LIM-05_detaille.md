# FT-LIM-05 — Cas génériques

## Préambule

Toujours lire `total_campaign_count` immédiatement avant la campagne de tests et noter `N`. Les tests dépendant d’un N non disponible sont N/A, jamais simulés par modification d’un registre RO.

## LIM05-G01 — Inventaire vide
Précondition : N=0.
Écrire index 0.
Attendu : écriture Modbus acceptée, aucune exception due à la valeur, `selected_campaign_valid=0`.

## LIM05-G02 — Première campagne
Précondition : N>0.
Écrire index 0.
Attendu : `selected_campaign_valid=1`, métadonnées valides, `campaign_id!=0`.

## LIM05-G03 — Dernière campagne
Précondition : N>0.
Écrire index N-1.
Attendu : sélection valide et `campaign_id!=0`.

## LIM05-G04 — Index intérieur
Précondition : N>2.
Choisir un index k tel que 0<k<N-1.
Attendu : sélection valide et métadonnées correspondant à k.

## LIM05-G05 — Première valeur hors plage
Précondition : N<=65535.
Écrire index N.
Attendu : écriture acceptée, `selected_campaign_valid=0`, aucune exception Modbus due à la valeur.

Remarque : pour N=0, ce cas coïncide numériquement avec G01 ; l’index actif ne doit pas être compté deux fois dans une même exécution de couverture.

## LIM05-G06 — Borne uint16
Précondition : N<65536 et 65535 hors domaine, ce qui est le cas pour tout N représentable <=65535.
Écrire 65535.
Attendu : écriture acceptée et `selected_campaign_valid=0`.

## LIM05-G07 — Métadonnées non valides après sélection invalide
Après G05 ou G06, vérifier `selected_campaign_valid=0` avant toute interprétation de 6012..6057. Ne pas imposer de remise à zéro ou de conservation des métadonnées : non défini.

## LIM05-G08 — Changement entre deux sélections valides
Précondition : N>=2.
Sélectionner i puis j, i!=j, et lire l’entrée après chaque sélection.
Attendu : chaque réponse représente une seule campagne ; aucun mélange de métadonnées. La preuve détaillée de cohérence multi-registres reste FT-STR.

## LIM05-G09 — Répétabilité d’une sélection valide
Précondition : N>0.
Sélectionner le même index valide et relire les métadonnées sans mutation d’inventaire connue.
Attendu : sélection toujours valide ; `campaign_id` non nul et cohérent avec la campagne sélectionnée.

## LIM05-G10 — Frontière N=65535
Précondition : N=65535.
Index 65534 valide ; index 65535 hors plage et représentable.
Attendu : bascule de `selected_campaign_valid` de 1 à 0 selon l’index.
