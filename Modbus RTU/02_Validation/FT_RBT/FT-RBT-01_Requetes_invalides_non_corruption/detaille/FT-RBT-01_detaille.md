# FT-RBT-01 — Cas de test détaillé

## TT-RBT-GEN-001 — Requête invalide intercalée entre échanges valides

**Objectif**  
Vérifier qu'une requête Modbus invalide, déjà qualifiée comme telle par FT-ACC, n'introduit pas de corruption observable empêchant l'application ultérieure des oracles nominaux.

**Sources normatives**  
- charte de typage §14 ;
- plan maître §2.5 ;
- oracle nominal de la zone choisie pour l'état de référence et la vérification finale.

**Propriété des oracles**  
- qualification de la requête comme invalide et exception attendue : FT-ACC ;
- absence d'effet de bord élémentaire : FT-ACC ;
- robustesse de la séquence `nominal → perturbation → nominal` : FT-RBT-01.

**Préconditions**
- sélectionner une zone dont les observables nécessaires peuvent être lus de façon déterministe ;
- sélectionner une requête invalide dont la qualification ne dépend d'aucune ambiguïté V1, par exemple une écriture sur registre strictement RO ;
- éviter d'utiliser comme témoin principal un champ légitimement dynamique dont la variation normale pourrait être confondue avec une corruption.

**Étapes**
1. effectuer un échange valide de référence et conserver les observables stables pertinents ;
2. injecter une requête invalide déterministe ;
3. constater son rejet selon l'oracle FT-ACC, sans faire du code d'exception exact un critère propriétaire FT-RBT ;
4. relire les observables pertinents afin de rechercher un effet interdit de la requête rejetée ;
5. effectuer un nouvel échange valide sur la même zone ou une zone choisie à l'avance ;
6. appliquer à ce nouvel échange exactement l'oracle nominal propriétaire de la famille concernée.

**Résultats attendus**
- aucun effet de bord interdit attribuable à la requête invalide n'est observé ;
- aucune exécution partielle interdite n'est observée ;
- le nouvel échange valide reste interprétable et vérifiable avec son oracle nominal ;
- aucun délai maximal de reprise n'est exigé ;
- aucun état interne de récupération non spécifié n'est exigé.

**Critère PASS**  
PASS si la perturbation n'introduit aucun écart par rapport aux garanties V1 déjà applicables à l'état et à l'échange valide ultérieur.

**Critère FAIL**  
FAIL si la requête rejetée provoque un effet de bord interdit, une exécution partielle, ou laisse apparaître une corruption observable violant un oracle V1 applicable.

**Ne constitue pas un FAIL FT-RBT-01**
- l'absence d'un temps de réponse inférieur à une valeur arbitraire ;
- l'absence d'une politique de retry particulière ;
- la variation normale d'un champ dynamique entre deux lectures séparées ;
- un code d'exception incorrect, qui constitue d'abord un verdict FT-ACC ;
- un comportement de trame CRC erroné ou tronquée, hors périmètre de ce cas.

**Mode d'exécution**  
Automatisable sur simulateur déterministe puis répétable sur matériel réel lorsque disponible.

**Traces à conserver**
- requête/réponse valide initiale ;
- requête invalide et réponse associée ;
- lectures de contrôle après rejet ;
- requête/réponse valide finale ;
- identification de l'oracle nominal réutilisé.

**Criticité**  
P0 — absence de corruption après sollicitation invalide.

---

## Points volontairement non instanciés

Aucun test autonome V1 n'est créé dans FT-RBT-01 pour :
- un temps maximal de récupération ;
- une rafale de N accès invalides ;
- une fréquence maximale de requêtes invalides ;
- un reset/watchdog après erreur ;
- des trames CRC incorrectes ou tronquées.

Ces comportements n'ont pas d'oracle V1 suffisamment défini dans le périmètre audité.
